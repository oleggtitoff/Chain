/*
 * EQ.c
 *
 *  Created on: Sep 10, 2019
 *      Author: Intern_2
 */

#include "EQ.h"

Status EQInit(EQCoeffs *coeffs, EQStates *states)
{
	int i;
	Status status = 0;

	for (i = 0; i < EQ_BANDS_NUM; i++)
	{
		status |= BiquadFilterInit(&coeffs->biquadCoeffs[i], &states->biquadStates[i]);
	}

	return status;
}

Status EQUpdateIsActive(EQCoeffs *coeffs, int8_t isActive)
{
	if (!coeffs)
		return statusError;

	coeffs->isActive = isActive;

	return statusOK;
}

Status EQUpdateBandCoeffs(EQCoeffs *coeffs, const int8_t band, BiquadCoeffs *newCoeffs)
{
	if (!coeffs || !newCoeffs)
		return statusError;

	coeffs->biquadCoeffs[band] = *newCoeffs;
	return statusOK;
}

F24x2 EQ_Process(const EQCoeffs *coeffs, EQStates *states, F24x2 sample)
{
	uint8_t i;

	if (coeffs->isActive)
	{
		for (i = 0; i < EQ_BANDS_NUM; i++)
		{
			if (coeffs->biquadCoeffs[i].isActive)
			{
				sample = BiquadFilter_Process(sample, &coeffs->biquadCoeffs[i],
									  &states->biquadStates[i]);
			}
		}
	}

	return sample;
}
