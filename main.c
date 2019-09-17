/*
 * main.c
 *
 *  Created on: Sep 2, 2019
 *      Author: Intern_2
 */

#include "InternalTypesArithmetic.h"
#include "ExternalAndInternalTypesConverters.h"

#include "CrossFade.h"
#include "EQ.h"
#include "AmplitudeProc.h"

#define SAMPLE_RATE 48000
#define BYTES_PER_SAMPLE 4
#define RING_BUFF_SIZE 		128
#define DATA_BUFF_SIZE 		512
#define CHANNELS 2

#define HEADROOM 4

#include "FilesOperations.h"

#define B0_IA 3
#define B0_A0 0.8155907170516708
#define B0_A1 -1.6311814341033417
#define B0_A2 0.8155907170516708
#define B0_B0 -1.596889108608175
#define B0_B1 0.6654737595985087

#define B1_IA 3
#define B1_A0 1.150764271403816
#define B1_A1 -1.540880814951698
#define B1_A2 0.6834133720269015
#define B1_B0 -1.540880814951698
#define B1_B1 0.8341776434307174

#define B2_IA 3
#define B2_A0 1.1115005479954862
#define B2_A1 -1.1604340662233836
#define B2_A2 0.6644368178255579
#define B2_B0 -1.1604340662233836
#define B2_B1 0.7759373658210442

#define B3_IA 3
#define B3_A0 1.03410921826377
#define B3_A1 -0.7187490455781434
#define B3_A2 0.7024232601251403
#define B3_B0 -0.7187490455781434
#define B3_B1 0.7365324783889103

#define B4_IA 3
#define B4_A0 1.1409627900708447
#define B4_A1 -0.24188970825362482
#define B4_A2 0.5757695970581652
#define B4_B0 -0.24188970825362482
#define B4_B1 0.7167323871290101

#define B5_IA 3
#define B5_A0 1.1409172898258763
#define B5_A1 0.24635155154392177
#define B5_A2 0.5759065309770185
#define B5_B0 0.24635155154392177
#define B5_B1 0.7168238208028951

#define B6_IA 3
#define B6_A0 0.8841938342004105
#define B6_A1 0.6392710927867951
#define B6_A2 0.6514789728862642
#define B6_B0 0.6392710927867951
#define B6_B1 0.5356728070866747

#define B7_IA 3
#define B7_A0 0.8998661258574124
#define B7_A1 1.0476645014787438
#define B7_A2 0.6986450555193505
#define B7_B0 1.0476645014787438
#define B7_B1 0.598511181376763

#define B8_IA 3
#define B8_A0 1.0822111735427564
#define B8_A1 1.5439997444323536
#define B8_A2 0.7525837899431729
#define B8_B0 1.5439997444323536
#define B8_B1 0.8347949634859291

#define B9_IA 3
#define B9_A0 0.8578798304496036
#define B9_A1 1.3730615923517604
#define B9_A2 0.5730485057782036
#define B9_B0 1.3104996085768296
#define B9_B1 0.493490320002738

#define EQ_IS_ACTIVE 3


#define NOISE_GATE_IS_ACTIVE 0
#define EXPANDER_IS_ACTIVE 	 0
#define COMPRESSOR_IS_ACTIVE 0
#define LIMITER_IS_ACTIVE 	 3

#define NOISE_GATE_THRESHOLD 0x0000a5cb
#define EXPANDER_THRESHOLD 	 0x0392ced8
#define COMPRESSOR_THRESHOLD 0x047faccf
#define LIMITER_THRESHOLD 	 0x050c335d

#define EXPANDER_C1   0x04000000
#define EXPANDER_C2   0x0558c4b2
#define COMPRESSOR_C1 0x04000000
#define COMPRESSOR_C2 0x05ffc889

#define ENVELOPE_ALPHA_ATTACK  0x7fffffff
#define ENVELOPE_ALPHA_RELEASE 0x0016bf66

#define EXPANDER_ALPHA_ATTACK  0x02a39acc
#define EXPANDER_ALPHA_RELEASE 0x02a39acc

#define COMPRESSOR_ALPHA_ATTACK  0x02a39acc
#define COMPRESSOR_ALPHA_RELEASE 0x02a39acc


#define INPUT_GAIN 	0x7fffffff
#define OUTPUT_GAIN 0x7fffffff
#define BALANCE_L 	0x7fffffff
#define BALANCE_R	0x7fffffff


typedef struct {
	F24x2 inputGain;				// Q1.23
	F24x2 outputGain;				// Q1.23
	F24x2 balance;					// Q1.23
	CrossFadeCoeffs crossFadeCoeffs;
	AmplitudeProcCoeffs amplitudeProcCoeffs;
	EQCoeffs eqCoeffs;
} Coeffs;

typedef struct {
	CrossFadeStates crossFadeStates;
	AmplitudeProcStates amplitudeProcStates;
	EQStates eqStates;
} States;


Status init(Coeffs *coeffs, States *states);
void updateCoeffs(Coeffs *coeffs);
void run(FILE *inputFilePtr, FILE *outputFilePtr,
						 Coeffs *coeffs, States *states);

int main()
{
	FILE *inputFilePtr = openFile("Sine_-12_1s.wav", binaryRead);
	FILE *outputFilePtr = openFile("Output.wav", binaryWrite);
	uint8_t headerBuff[FILE_HEADER_SIZE];
	readHeader(headerBuff, inputFilePtr);
	writeHeader(headerBuff, outputFilePtr);

	Coeffs coeffs;
	States states;

	init(&coeffs, &states);
	updateCoeffs(&coeffs);
	run(inputFilePtr, outputFilePtr, &coeffs, &states);

	fclose(inputFilePtr);
	fclose(outputFilePtr);
	return 0;
}


Status init(Coeffs *coeffs, States *states)
{
	coeffs->inputGain  = IntToF24x2Set(0x7fffffff);
	coeffs->outputGain = IntToF24x2Set(0x7fffffff);
	coeffs->balance    = IntToF24x2Set(0x0);

	CrossFadeInit(&coeffs->crossFadeCoeffs, &states->crossFadeStates);
	AmplitudeProcInit(&coeffs->amplitudeProcCoeffs, &states->amplitudeProcStates);
	EQInit(&coeffs->eqCoeffs, &states->eqStates);
}

void updateCrossFadeCoeffs(CrossFadeCoeffs *coeffs)
{
	CrossFadeCoeffs newCoeffs;
	newCoeffs.targetGain  = IntToF24x2Set(0x0);
	newCoeffs.fadeAlpha   = IntToF24x2Set(0x00616100);
	newCoeffs.fadeAlpha2  = IntToF24x2Set(0x7f9e9e00);

	CrossFadeUpdateCoeffs(coeffs, &newCoeffs);
}

void updateEQCoeffs(EQCoeffs *coeffs)
{
	EQUpdateIsActive(coeffs, EQ_IS_ACTIVE);

	BiquadCoeffs newCoeffs;
	newCoeffs.isActive = (int8_t)B0_IA;
	newCoeffs.a[0] 	= DoubleToF24x2Set(B0_A0 / 16);
	newCoeffs.a[1] 	= DoubleToF24x2Set(B0_A1 / 16);
	newCoeffs.a[2] 	= DoubleToF24x2Set(B0_A2 / 16);
	newCoeffs.b[0] 	= DoubleToF24x2Set(B0_B0 / 16);
	newCoeffs.b[1] 	= DoubleToF24x2Set(B0_B1 / 16);
	EQUpdateBandCoeffs(coeffs, 0, &newCoeffs);

	newCoeffs.isActive = (int8_t)B1_IA;
	newCoeffs.a[0] 	= DoubleToF24x2Set(B1_A0 / 16);
	newCoeffs.a[1] 	= DoubleToF24x2Set(B1_A1 / 16);
	newCoeffs.a[2] 	= DoubleToF24x2Set(B1_A2 / 16);
	newCoeffs.b[0] 	= DoubleToF24x2Set(B1_B0 / 16);
	newCoeffs.b[1] 	= DoubleToF24x2Set(B1_B1 / 16);
	EQUpdateBandCoeffs(coeffs, 1, &newCoeffs);

	newCoeffs.isActive = (int8_t)B2_IA;
	newCoeffs.a[0] 	= DoubleToF24x2Set(B2_A0 / 16);
	newCoeffs.a[1] 	= DoubleToF24x2Set(B2_A1 / 16);
	newCoeffs.a[2] 	= DoubleToF24x2Set(B2_A2 / 16);
	newCoeffs.b[0] 	= DoubleToF24x2Set(B2_B0 / 16);
	newCoeffs.b[1] 	= DoubleToF24x2Set(B2_B1 / 16);
	EQUpdateBandCoeffs(coeffs, 2, &newCoeffs);

	newCoeffs.isActive = (int8_t)B3_IA;
	newCoeffs.a[0] 	= DoubleToF24x2Set(B3_A0 / 16);
	newCoeffs.a[1] 	= DoubleToF24x2Set(B3_A1 / 16);
	newCoeffs.a[2] 	= DoubleToF24x2Set(B3_A2 / 16);
	newCoeffs.b[0] 	= DoubleToF24x2Set(B3_B0 / 16);
	newCoeffs.b[1] 	= DoubleToF24x2Set(B3_B1 / 16);
	EQUpdateBandCoeffs(coeffs, 3, &newCoeffs);

	newCoeffs.isActive = (int8_t)B4_IA;
	newCoeffs.a[0] 	= DoubleToF24x2Set(B4_A0 / 16);
	newCoeffs.a[1] 	= DoubleToF24x2Set(B4_A1 / 16);
	newCoeffs.a[2] 	= DoubleToF24x2Set(B4_A2 / 16);
	newCoeffs.b[0] 	= DoubleToF24x2Set(B4_B0 / 16);
	newCoeffs.b[1] 	= DoubleToF24x2Set(B4_B1 / 16);
	EQUpdateBandCoeffs(coeffs, 4, &newCoeffs);

	newCoeffs.isActive = (int8_t)B5_IA;
	newCoeffs.a[0] 	= DoubleToF24x2Set(B5_A0 / 16);
	newCoeffs.a[1] 	= DoubleToF24x2Set(B5_A1 / 16);
	newCoeffs.a[2] 	= DoubleToF24x2Set(B5_A2 / 16);
	newCoeffs.b[0] 	= DoubleToF24x2Set(B5_B0 / 16);
	newCoeffs.b[1] 	= DoubleToF24x2Set(B5_B1 / 16);
	EQUpdateBandCoeffs(coeffs, 5, &newCoeffs);

	newCoeffs.isActive = (int8_t)B6_IA;
	newCoeffs.a[0] 	= DoubleToF24x2Set(B6_A0 / 16);
	newCoeffs.a[1] 	= DoubleToF24x2Set(B6_A1 / 16);
	newCoeffs.a[2] 	= DoubleToF24x2Set(B6_A2 / 16);
	newCoeffs.b[0] 	= DoubleToF24x2Set(B6_B0 / 16);
	newCoeffs.b[1] 	= DoubleToF24x2Set(B6_B1 / 16);
	EQUpdateBandCoeffs(coeffs, 6, &newCoeffs);

	newCoeffs.isActive = (int8_t)B7_IA;
	newCoeffs.a[0] 	= DoubleToF24x2Set(B7_A0 / 16);
	newCoeffs.a[1] 	= DoubleToF24x2Set(B7_A1 / 16);
	newCoeffs.a[2] 	= DoubleToF24x2Set(B7_A2 / 16);
	newCoeffs.b[0] 	= DoubleToF24x2Set(B7_B0 / 16);
	newCoeffs.b[1] 	= DoubleToF24x2Set(B7_B1 / 16);
	EQUpdateBandCoeffs(coeffs, 7, &newCoeffs);

	newCoeffs.isActive = (int8_t)B8_IA;
	newCoeffs.a[0] 	= DoubleToF24x2Set(B8_A0 / 16);
	newCoeffs.a[1] 	= DoubleToF24x2Set(B8_A1 / 16);
	newCoeffs.a[2] 	= DoubleToF24x2Set(B8_A2 / 16);
	newCoeffs.b[0] 	= DoubleToF24x2Set(B8_B0 / 16);
	newCoeffs.b[1] 	= DoubleToF24x2Set(B8_B1 / 16);
	EQUpdateBandCoeffs(coeffs, 8, &newCoeffs);

	newCoeffs.isActive = (int8_t)B9_IA;
	newCoeffs.a[0] 	= DoubleToF24x2Set(B9_A0 / 16);
	newCoeffs.a[1] 	= DoubleToF24x2Set(B9_A1 / 16);
	newCoeffs.a[2] 	= DoubleToF24x2Set(B9_A2 / 16);
	newCoeffs.b[0] 	= DoubleToF24x2Set(B9_B0 / 16);
	newCoeffs.b[1] 	= DoubleToF24x2Set(B9_B1 / 16);
	EQUpdateBandCoeffs(coeffs, 9, &newCoeffs);
}

void updateAmplitudeProcCoeffs(AmplitudeProcCoeffs *coeffs)
{
	AmplitudeProcCoeffs newCoeffs;

	newCoeffs.noiseGate.isActive = NOISE_GATE_IS_ACTIVE;
	newCoeffs.expander.isActive = EXPANDER_IS_ACTIVE;
	newCoeffs.compressor.isActive = COMPRESSOR_IS_ACTIVE;
	newCoeffs.limiter.isActive = LIMITER_IS_ACTIVE;

	newCoeffs.noiseGate.threshold = IntToF24x2Set(NOISE_GATE_THRESHOLD);
	newCoeffs.expander.threshold = IntToF24x2Set(EXPANDER_THRESHOLD);
	newCoeffs.compressor.threshold = IntToF24x2Set(COMPRESSOR_THRESHOLD);
	newCoeffs.limiter.threshold = IntToF24x2Set(LIMITER_THRESHOLD);

	newCoeffs.expander.C1 = IntToF24x2Set(EXPANDER_C1);
	newCoeffs.expander.C2 = IntToF24x2Set(EXPANDER_C2);
	newCoeffs.compressor.C1 = IntToF24x2Set(COMPRESSOR_C1);
	newCoeffs.compressor.C2 = IntToF24x2Set(COMPRESSOR_C2);

	newCoeffs.envelope.alphaAttack = IntToF24x2Set(ENVELOPE_ALPHA_ATTACK);
	newCoeffs.envelope.alphaRelease = IntToF24x2Set(ENVELOPE_ALPHA_RELEASE);

	newCoeffs.expander.alphaAttack = IntToF24x2Set(EXPANDER_ALPHA_ATTACK);
	newCoeffs.expander.alphaRelease = IntToF24x2Set(EXPANDER_ALPHA_RELEASE);

	newCoeffs.compressor.alphaAttack = IntToF24x2Set(COMPRESSOR_ALPHA_ATTACK);
	newCoeffs.compressor.alphaRelease = IntToF24x2Set(COMPRESSOR_ALPHA_RELEASE);

	AmplitudeProcUpdateCoeffs(coeffs, &newCoeffs);
}

void updateCoeffs(Coeffs *coeffs)
{
	coeffs->inputGain  = IntToF24x2Set(INPUT_GAIN);
	coeffs->outputGain = IntToF24x2Set(OUTPUT_GAIN);
	coeffs->balance    = IntToF24x2Join(BALANCE_L, BALANCE_R);

	updateEQCoeffs(&coeffs->eqCoeffs);
	updateAmplitudeProcCoeffs(&coeffs->amplitudeProcCoeffs);
}

void run(FILE *inputFilePtr, FILE *outputFilePtr, Coeffs *coeffs,
		 States *states)
{
	int32_t dataBuff[DATA_BUFF_SIZE * CHANNELS];
	size_t samplesRead;
	uint32_t i;
	F24x2 bypassSample;				// 1.23
	F24x2 sample;					// Q5.19
	uint32_t cyclesCounter = 0;

	while (1)
	{
		samplesRead = fread(dataBuff, BYTES_PER_SAMPLE, DATA_BUFF_SIZE * CHANNELS, inputFilePtr);

		if (!samplesRead)
		{
			break;
		}

//// CROSSFADE TEST HERE
		if (cyclesCounter == 40)//300)
		{
			updateCrossFadeCoeffs(&coeffs->crossFadeCoeffs);
		}

		for (i = 0; i < samplesRead / CHANNELS; i++)
		{
			bypassSample = IntToF24x2Join(dataBuff[i * CHANNELS], dataBuff[i * CHANNELS]);
			sample = F24x2RightShiftA(bypassSample, HEADROOM);
			//sample = F24x2Mul(sample, coeffs->inputGain);
			//sample = F24x2Mul(sample, coeffs->balance);

			//sample = EQ_Process(&coeffs->eqCoeffs, &states->eqStates, sample);
			//sample = AmplitudeProc_Process(&coeffs->amplitudeProcCoeffs,
			//							   &states->amplitudeProcStates, sample);

			//sample = F24x2Mul(sample, coeffs->outputGain);
			sample = F24x2LeftShiftAS(sample, HEADROOM);

			sample = CrossFade_Process(&coeffs->crossFadeCoeffs,
									   &states->crossFadeStates,
									   bypassSample, sample);

			dataBuff[i * CHANNELS + 1] = F24x2ToIntExtract_l(sample);
		}

		fwrite(dataBuff, BYTES_PER_SAMPLE, samplesRead, outputFilePtr);

		cyclesCounter++;
	}
}

//ALWAYS_INLINE Status initCoeffs(Coeffs *coeffs)
//{
//	if (!coeffs)
//		return statusError;
//
//	coeffs->inputGain = F32x2Zero();
//	coeffs->outputGain = F32x2Zero();
//	coeffs->balance = F32x2Set(0x7fffffff);
//
//	return statusOK;
//}
//
//ALWAYS_INLINE Status initStates(States *states)
//{
//	//TODO
//}
//
//ALWAYS_INLINE Status init(Coeffs *coeffs, States *states)
//{
//	//TODO
//}
