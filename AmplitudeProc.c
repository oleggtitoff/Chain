/*
 * AmplitudeProc.c
 *
 *  Created on: Sep 10, 2019
 *      Author: Intern_2
 */

#include "AmplitudeProc.h"

ALWAYS_INLINE Status coeffsInit(AmplitudeProcCoeffs *coeffs)
{
	// initializes coefficients into initial state

	if (!coeffs)
		return statusError;

	coeffs->noiseGate.isActive 	 	= 0;
	coeffs->expander.isActive 		= 0;
	coeffs->compressor.isActive 	= 0;
	coeffs->limiter.isActive 		= 0;

	coeffs->noiseGate.threshold 	= F24x2Zero();
	coeffs->expander.threshold 	 	= F24x2Zero();
	coeffs->compressor.threshold 	= F24x2Zero();
	coeffs->limiter.threshold 		= F24x2Zero();

	coeffs->expander.C1 			= F24x2Zero();
	coeffs->expander.C2 			= F24x2Zero();
	coeffs->compressor.C1 			= F24x2Zero();
	coeffs->compressor.C2 			= F24x2Zero();

	coeffs->envelope.alphaAttack 	= F24x2Zero();
	coeffs->envelope.alphaRelease 	= F24x2Zero();
	coeffs->expander.alphaAttack 	= F24x2Zero();
	coeffs->expander.alphaRelease 	= F24x2Zero();
	coeffs->compressor.alphaAttack  = F24x2Zero();
	coeffs->compressor.alphaRelease = F24x2Zero();

	return statusOK;
}

ALWAYS_INLINE Status rignBuffInit(RingBuff *ringBuff)
{
	// initializes ring buffer into initial state

	if (!ringBuff)
		return statusError;

	int16_t i;
	ringBuff->currNum   = 0;
	ringBuff->maxSample = F24x2Zero();

	for (i = 0; i < RING_BUFF_SIZE; i++)
	{
		ringBuff->samples[i] = F24x2Zero();
	}

	return statusOK;
}

ALWAYS_INLINE Status statesInit(AmplitudeProcStates *states)
{
	// initializes states into initial state

	if (!states)
		return statusError;

	rignBuffInit(&states->ringBuff);

	states->envelope.prevSample = F24x2Zero();
	states->noiseGate.isWorked 	= (Boolx2)0;
	states->compressor.isWorked = (Boolx2)0;
	states->expander.isWorked 	= (Boolx2)0;
	states->compressor.prevGain = IntToF24x2Set(0x08000000);
	states->expander.prevGain 	= IntToF24x2Set(0x08000000);

	return statusOK;
}

ALWAYS_INLINE void updateMaxRingBuffValue(RingBuff *ringBuff)
{
	// calculates and updates maxSample value in ring buffer

	uint16_t i;
	ringBuff->maxSample = F24x2Abs(ringBuff->samples[0]);

	for (i = 1; i < RING_BUFF_SIZE; i++)
	{
		ringBuff->maxSample = F24x2Max(ringBuff->maxSample, F24x2Abs(ringBuff->samples[i]));
	}
}

ALWAYS_INLINE F24x2 smoothingFilter(const F24x2 in, const F24x2 alpha, const F24x2 prev)
{
	// gets current value, previous value and alpha coefficient
	// returns filtered value

	F24x2 alpha2  = F24x2Sub(IntToF24x2Set(0x7fffffff), alpha);

	F56x2 mulRes1 = F24x2MacF56x2(F56x2Zero(), in, alpha);
	F56x2 mulRes2 = F24x2MacF56x2(F56x2Zero(), alpha2, prev);
	F56x2 addRes  = F56x2Add(mulRes1, mulRes2);

	return F56x2ToF24x2_h(addRes);
}

//ALWAYS_INLINE
F24x2 envelopeCalc(const EnvelopeCoeffs *coeffs, EnvelopeStates *states,
								 const F24x2 sample)
{
	// calculates signal envelope

	F24x2 sampleABS = F24x2Abs(sample);			// Q5.19
	F24x2 alpha = coeffs->alphaAttack;
	Boolx2 isRelease = F24x2LessThan(sampleABS, F24x2Abs(states->prevSample));
	F24x2MovIfTrue(&alpha, coeffs->alphaRelease, isRelease);
	states->prevSample = smoothingFilter(sampleABS, alpha, states->prevSample);

	return states->prevSample;
}

ALWAYS_INLINE F24x2 instantGainCalc(const F24x2 envelope, const F24x2 C1, const F24x2 C2)
{
	// calculates instant gain value

	F24x2 negC1 	  	= F24x2Sub(F24x2Zero(), C1);
	F24x2 powRes	  	= F24x2Pow(envelope, negC1);
	F24x2 mulRes	 	= F24x2Mul(C2, powRes);
	F24x2 instantGain 	= F24x2LeftShiftAS(mulRes, 4);		// Q5.19

	return instantGain;
}

ALWAYS_INLINE F24x2 gainSmoothing(const F24x2 gain, const F24x2 alphaAttack,
								  const F24x2 alphaRelease, F24x2 *prevGain)
{
	F24x2 alpha = alphaAttack;
	F24x2MovIfTrue(&alpha, alphaRelease, F24x2LessThan(*prevGain, gain));

	return smoothingFilter(gain, alpha, *prevGain);
}

//ALWAYS_INLINE
static F24x2 noiseGate(const NoiseGateCoeffs *coeffs, NoiseGateStates *states, F24x2 envelope)
{
	// changes gain to zero, if envelope sample is under the threshold

	F24x2 gain = IntToF24x2Set(0x08000000);		// Q5.19
	Boolx2 isNoiseGate = F24x2LessThan(envelope, coeffs->threshold);
	isNoiseGate = Boolx2AND(isNoiseGate, (Boolx2)coeffs->isActive);
	F24x2MovIfTrue(&gain, F24x2Zero(), isNoiseGate);
	states->isWorked = isNoiseGate;

	return gain;
}

ALWAYS_INLINE F24x2 dynamicRangeCore(const CompressorCoeffs *coeffs, CompressorStates *states,
									const F24x2 envelope)
{
	F24x2 gain = instantGainCalc(envelope, coeffs->C1, coeffs->C2);		// Q5.19
	gain = gainSmoothing(gain, coeffs->alphaAttack, coeffs->alphaRelease, &states->prevGain);
	return gain;
}

//ALWAYS_INLINE
static F24x2 expander(const ExpanderCoeffs *coeffs, ExpanderStates *states,
							  const F24x2 envelope)
{
	F24x2 gain = IntToF24x2Set(0x08000000);		// Q5.19
	states->isWorked = (Boolx2)0;

	if (coeffs->isActive)
	{
		F24x2 expGain = dynamicRangeCore(coeffs, states, envelope);
		Boolx2 isExpander = F24x2LessThan(envelope, coeffs->threshold);
		F24x2MovIfTrue(&gain, expGain, isExpander);
		states->isWorked = isExpander;
	}

	states->prevGain = gain;
	return gain;
}

//ALWAYS_INLINE
static F24x2 compressor(const CompressorCoeffs *coeffs, CompressorStates *states,
							   const F24x2 envelope)
{
	F24x2 gain = IntToF24x2Set(0x08000000);		// Q5.19
	states->isWorked = (Boolx2)0;

	if (coeffs->isActive)
	{
		F24x2 comprGain = dynamicRangeCore(coeffs, states, envelope);
		Boolx2 isCompressor = F24x2LessThan(coeffs->threshold, envelope);
		F24x2MovIfTrue(&gain, comprGain, isCompressor);
		states->isWorked = isCompressor;
	}

	states->prevGain = gain;
	return gain;
}

//ALWAYS_INLINE
static F24x2 limiter(const LimiterCoeffs *coeffs, RingBuff *ringBuff)
{
	// changes gain to new limited gain, if maxSample in ring buffer with applied
	// current gain is over the threshold

	F24x2 gain = IntToF24x2Set(0x08000000);		// Q5.19

	if (coeffs->isActive)
	{
		updateMaxRingBuffValue(ringBuff);
		Boolx2 isLimiter  = F24x2LessThan(coeffs->threshold, ringBuff->maxSample);
		F24x2 limGain = F24x2RightShiftA(F24x2Div(coeffs->threshold, ringBuff->maxSample), 4);
		F24x2MovIfTrue(&gain, limGain, isLimiter);
	}

	return gain;
}

Status AmplitudeProcInit(AmplitudeProcCoeffs *coeffs, AmplitudeProcStates *states)
{
	// initializes coefficients and buffers into well-defined initial state

	Status status = 0;

	status |= coeffsInit(coeffs);
	status |= statesInit(states);

	return status;
}

Status AmplitudeProcUpdateCoeffs(AmplitudeProcCoeffs *coeffs, AmplitudeProcCoeffs *newCoeffs)
{
	if (!coeffs || !newCoeffs)
		return statusError;

	*coeffs = *newCoeffs;
	return statusOK;
}

ALWAYS_INLINE void ringBuffAddValue(RingBuff *ringBuff, F24x2 value)
{
	ringBuff->samples[ringBuff->currNum] = value;
	ringBuff->currNum = (ringBuff->currNum + 1) & (RING_BUFF_SIZE - 1);
}

F24x2 AmplitudeProc_Process(const AmplitudeProcCoeffs *coeffs, AmplitudeProcStates *states,
						   const F24x2 sample)
{
	F24x2 gain 		   = IntToF24x2Set(INT32_MAX);										// Q5.19
	Boolx2 isProcessed = (Boolx2)0;
	F24x2 envelope	   = envelopeCalc(&coeffs->envelope, &states->envelope, sample);	// Q5.19
	F24x2 res;																			// Q5.19

	// NOISE GATE
	F24x2 tmpGain = noiseGate(&coeffs->noiseGate, &states->noiseGate, envelope);
	F24x2MovIfTrue(&gain, tmpGain, states->noiseGate.isWorked);

	// EXPANDER
	tmpGain = expander(&coeffs->expander, &states->expander, envelope);
	F24x2MovIfTrue(&gain, F24x2Min(gain, tmpGain), states->expander.isWorked);

	// COMPRESSOR
	tmpGain = compressor(&coeffs->compressor, &states->compressor, envelope);
	F24x2MovIfTrue(&gain, F24x2Min(gain, tmpGain), states->compressor.isWorked);

	// if nothing was applied, gain sets to "1"
	isProcessed = Boolx2OR(states->noiseGate.isWorked, states->expander.isWorked);
	isProcessed = Boolx2OR(isProcessed, states->compressor.isWorked);
	F24x2MovIfTrue(&gain, IntToF24x2Set(0x08000000), Boolx2NOT(isProcessed));

	// gain applies to sample
	res = F24x2Mul(gain, sample);
	res = F24x2LeftShiftAS(res, 4);
	ringBuffAddValue(&states->ringBuff, res);

	// LIMITER
	tmpGain = limiter(&coeffs->limiter, &states->ringBuff);
	res = F24x2Mul(states->ringBuff.samples[states->ringBuff.currNum], tmpGain);
	res = F24x2LeftShiftAS(res, 4);

	return res;
}
