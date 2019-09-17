/*
 * BiquadFilter.c
 *
 *  Created on: Sep 6, 2019
 *      Author: Intern_2
 */

#include "BiquadFilter.h"

#include "InternalTypesConverters.h"
#include "ExternalAndInternalTypesConverters.h"

ALWAYS_INLINE Status biquadCoeffsInit(BiquadCoeffs *coeffs)
{
	if (!coeffs)
		return statusError;

	coeffs->isActive = 0;
	coeffs->a[0] = F24x2Zero();
	coeffs->a[1] = F24x2Zero();
	coeffs->a[2] = F24x2Zero();
	coeffs->b[0] = F24x2Zero();
	coeffs->b[1] = F24x2Zero();

	return statusOK;
}

ALWAYS_INLINE Status biquadStatesReset(BiquadStates *states)
{
	if (!states)
		return statusError;

	states->x[0] = F24x2Zero();
	states->x[1] = F24x2Zero();
	states->y[0] = F24x2Zero();
	states->y[1] = F24x2Zero();
	states->err  = F24x2Zero();

	return statusOK;
}


Status BiquadFilterInit(BiquadCoeffs *coeffs, BiquadStates *states)
{
	Status status = 0;

	status |= biquadCoeffsInit(coeffs);
	status |= biquadStatesReset(states);

	return status;
}

Status BiquadFilterUpdateCoeffs(BiquadCoeffs *coeffs, BiquadCoeffs *newCoeffs)
{
	if (!coeffs || !newCoeffs)
		return statusError;

	*coeffs = *newCoeffs;
	return statusOK;
}

F24x2 BiquadFilter_Process(const F24x2 sample, const BiquadCoeffs *coeffs,
								 BiquadStates *states)
{
	F56x2 acc = F24x2ToF56x2_l(states->err);

	acc = F24x2MacF56x2(acc, coeffs->a[0], sample);
	acc = F24x2MSubF56x2(acc, coeffs->b[0], states->y[0]);
	acc = F24x2MacF56x2(acc, coeffs->a[1], states->x[0]);
	acc = F24x2MSubF56x2(acc, coeffs->b[1], states->y[1]);
	acc = F24x2MacF56x2(acc, coeffs->a[2], states->x[1]);

	states->err = F56x2ToF24x2_l(F56x2AND(acc, F24x2ToF56x2_l(IntToF24x2Set(0xFFFFF00LL))));

	acc = F56x2LeftShiftAS(acc, 4);

	states->x[1] = states->x[0];
	states->x[0] = sample;
	states->y[1] = states->y[0];
	states->y[0] = F56x2ToF24x2_h(acc);

	return states->y[0];
}

