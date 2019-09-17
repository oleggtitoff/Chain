/*
 * CrossFade.c
 *
 *  Created on: Sep 6, 2019
 *      Author: Intern_2
 */

#include "CrossFade.h"

ALWAYS_INLINE Status fadeCoeffsInit(CrossFadeCoeffs *coeffs)
{
	if (!coeffs)
		return statusError;

	coeffs->targetGain = IntToF24x2Set(0x7fffff00);
	coeffs->fadeAlpha  = IntToF24x2Set(0x00616100);
	coeffs->fadeAlpha2 = IntToF24x2Set(0x7f9e9e00);

	return statusOK;
}

ALWAYS_INLINE Status fadeStatesInit(CrossFadeStates *states)
{
	if (!states)
		return statusError;

	states->prevGain = IntToF24x2Set(0x7fffff00);
	states->gainErr  = F24x2Zero();
	states->procErr  = F24x2Zero();

	return statusOK;
}

//ALWAYS_INLINE
static F24x2 calcFadeGain(const CrossFadeCoeffs *coeffs, CrossFadeStates *states)
{
	F56x2 error	  	 = F24x2ToF56x2_l(states->gainErr);
	F56x2 mulRes1	 = F24x2MacF56x2(F56x2Zero(), coeffs->targetGain, coeffs->fadeAlpha);
	F56x2 mulRes2 	 = F24x2MacF56x2(error, coeffs->fadeAlpha2, states->prevGain);
	F56x2 addRes 	 = F56x2Add(mulRes1, mulRes2);

	states->gainErr  = F56x2ToF24x2_l(F56x2AND(addRes, F24x2ToF56x2_l(IntToF24x2Set(0xffffff00))));
	states->prevGain = F56x2ToF24x2_h(addRes);


	F24x2 subRes1 = F24x2Sub(coeffs->targetGain, states->prevGain);
	F24x2 subAbs1 = F24x2Abs(subRes1);
	F24x2 precision = IntToF24x2Set(0x1ff00);
	Boolx2 lessRes1 = F24x2LessEqual(subAbs1, precision);
	F24x2MovIfTrue(&states->prevGain, coeffs->targetGain, lessRes1);

	return states->prevGain;
}

Status CrossFadeInit(CrossFadeCoeffs *coeffs, CrossFadeStates *states)
{
	Status status = 0;

	status |= fadeCoeffsInit(coeffs);
	status |= fadeStatesInit(states);

	return status;
}

Status CrossFadeUpdateCoeffs(CrossFadeCoeffs *coeffs, CrossFadeCoeffs *newCoeffs)
{
	if (!coeffs || !newCoeffs)
		return statusError;

	*coeffs = *newCoeffs;
	return statusOK;
}

F24x2 CrossFade_Process(const CrossFadeCoeffs *coeffs, CrossFadeStates *states,
							  const F24x2 bypassSample, const F24x2 sample)
{
	F24x2 gain1   	= calcFadeGain(coeffs, states);
	F24x2 gain2   	= F24x2Sub(IntToF24x2Set(0x7fffff00), gain1);

	F56x2 error	  	= F24x2ToF56x2_l(states->procErr);
	F56x2 mulRes1 	= F24x2MacF56x2(F56x2Zero(), gain1, bypassSample);
	F56x2 mulRes2 	= F24x2MacF56x2(F56x2Zero(), gain2, sample);
	F56x2 addRes  	= F56x2Add(mulRes1, mulRes2);

	states->procErr = F56x2ToF24x2_l(F56x2AND(addRes, F24x2ToF56x2_l(IntToF24x2Set(0xFFFFFF00LL))));
	F24x2 res 		= F56x2ToF24x2_h(addRes);
	return res;
}
