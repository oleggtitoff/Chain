/*
 * EQ.h
 *
 *  Created on: Sep 10, 2019
 *      Author: Intern_2
 */

#ifndef EQ
#define EQ

#include "BiquadFilter.h"

#define EQ_BANDS_NUM 10

typedef struct {
	int8_t isActive;
	BiquadCoeffs biquadCoeffs[EQ_BANDS_NUM];
} EQCoeffs;

typedef struct {
	BiquadStates biquadStates[EQ_BANDS_NUM];
} EQStates;


Status EQInit(EQCoeffs *coeffs, EQStates *states);
Status EQUpdateBandCoeffs(EQCoeffs *coeffs, const int8_t band, BiquadCoeffs *newCoeffs);
F24x2 EQ_Process(const EQCoeffs *coeffs, EQStates *states, F24x2 sample);

#endif /* EQ */
