/*
 * CrossFade.h
 *
 *  Created on: Sep 2, 2019
 *      Author: Intern_2
 */

#ifndef CROSS_FADE
#define CROSS_FADE

#include "InternalTypesArithmetic.h"
#include "PredicatesOperations.h"
#include "InternalTypesConverters.h"

typedef struct {
	F24x2 targetGain;		// Q1.23
	F24x2 fadeAlpha;		// Q1.23
	F24x2 fadeAlpha2;
} CrossFadeCoeffs;

typedef struct {
	F24x2 prevGain;			// Q1.23
	F24x2 gainErr;
	F24x2 procErr;
} CrossFadeStates;


Status CrossFadeInit(CrossFadeCoeffs *coeffs,
		 CrossFadeStates *states);
Status CrossFadeSetCoeff(CrossFadeCoeffs *coeffs);
F24x2 CrossFade_Process(const CrossFadeCoeffs *coeffs, CrossFadeStates *states,
							  const F24x2 bypassSample, const F24x2 sample);


#endif /* CROSS_FADE */
