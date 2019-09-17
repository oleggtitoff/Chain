/*
 * BiquadFilter.h
 *
 *  Created on: Sep 6, 2019
 *      Author: Intern_2
 */

#ifndef BIQUAD_FILTER
#define BIQUAD_FILTER

#include "InternalTypesArithmetic.h"

typedef struct {
	uint8_t isActive;
	F24x2 a[3];			// Q5.19
	F24x2 b[2];			// Q5.19
} BiquadCoeffs;

typedef struct {
	F24x2 x[2];
	F24x2 y[2];
	F24x2 err;
} BiquadStates;


Status BiquadFilterInit(BiquadCoeffs *coeffs, BiquadStates *states);
Status BiquadFilterUpdateCoeffs(BiquadCoeffs *coeffs, BiquadCoeffs *newCoeffs);
F24x2 BiquadFilter_Process(const F24x2 sample, const BiquadCoeffs *coeffs,
								 BiquadStates *states);

#endif /* BIQUAD_FILTER */
