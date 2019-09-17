/*
 * InternalTypesConverters.h
 *
 *  Created on: Sep 6, 2019
 *      Author: Intern_2
 */

#ifndef INTERNAL_TYPES_CONVERTERS
#define INTERNAL_TYPES_CONVERTERS

#include "InternalTypesArithmetic.h"

//
// INTERNAL TYPES CONVERTERS
//
ALWAYS_INLINE F24x2 F56x2ToF24x2_h(const F56x2 x)
{
	// function gets Q9.47 and returns Q1.23

	return AE_TRUNCP24Q48X2(x.h, x.l);
}

ALWAYS_INLINE F24x2 F56x2ToF24x2_l(const F56x2 x)
{
	// function gets Q9.47 and returns low 24 bits

	return F56x2ToF24x2_h(F56x2LeftShiftL(x, 24));
}

ALWAYS_INLINE F24x2 F56x2RoundToF24x2(const F56x2 x)
{
	// function rounds Q9.47 value to Q1.23

	return AE_SELP24_HH(AE_ROUNDSP24Q48SYM(x.h), AE_ROUNDSP24Q48SYM(x.l));
}

ALWAYS_INLINE F56x2 F24x2ToF56x2_l(const F24x2 x)
{
	// function gets low 24 bits and puts in Q9.47

	F56x2 res;
	res.h = AE_CVTQ48P24S_H(x);
	res.l = AE_CVTQ48P24S_L(x);
	res = F56x2LeftShiftL(res, 8);
	res = F56x2RightShiftL(res, 32);
	return res;
}

#endif /* INTERNAL_TYPES_CONVERTERS */
