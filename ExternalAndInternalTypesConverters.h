/*
 * ExternalAndInternalTypesConverters.h
 *
 *  Created on: Sep 3, 2019
 *      Author: Intern_2
 */

#ifndef EXTERNAL_AND_INTERNAL_TYPES_CONVERTERS
#define EXTERNAL_AND_INTERNAL_TYPES_CONVERTERS

#include "InternalTypesDefinitions.h"

//
// EXTERNAL/INTERNAL TYPES CONVERTERS
//
ALWAYS_INLINE int32_t DoubleToInt32(const double x)
{
	// converts double in range -1 1 to int32 Q31 representation

	if (x >= 1)
	{
		return INT32_MAX;
	}
	else if (x < -1)
	{
		return INT32_MIN;
	}

	return (int32_t)(x * (double)(1LL << 31));
}

ALWAYS_INLINE double Int32ToDouble(const int32_t x)
{
	// converts int32 Q31 representation to double

	return (double)((int32_t)x / (double)(1LL << 31));
}


ALWAYS_INLINE F24x2 IntToF24x2Set(const int32_t x)
{
	// converts scalar 32-bit fractional to vector 24-bit fractional
	// duplicates the input value to both high and low parts of the result

	return AE_TRUNCP24A32X2(x, x);
}

ALWAYS_INLINE F24x2 IntToF24x2Join(const int32_t h, const int32_t l)
{
	// converts two scalar 32-bit fractions to vector 24-bit fractional

	return AE_TRUNCP24A32X2(h, l);
}

ALWAYS_INLINE F24x2 DoubleToF24x2Set(const double x)
{
	// converts scalar double to vector 24-bit fractional
	// duplicates the input value to both high and low parts of the result

	return IntToF24x2Set(DoubleToInt32(x));
}

ALWAYS_INLINE F24x2 DoubleToF24x2Join(const double h, const double l)
{
	// converts two scalar doubles to vector 24-bit fractional

	return IntToF24x2Join(DoubleToInt32(h), DoubleToInt32(l));
}


ALWAYS_INLINE int32_t F24x2ToIntExtract_h(const F24x2 x)
{
	// extracts high value from vector 24-bit fractional and converts it
	// to scalar 32-bit fractional

	return AE_CVTA32P24_H(x);
}

ALWAYS_INLINE int32_t F24x2ToIntExtract_l(const F24x2 x)
{
	// extracts low value from vector 24-bit fractional and converts it
	// to scalar 32-bit fractional

	return AE_CVTA32P24_L(x);
}

ALWAYS_INLINE double F24x2ToDoubleExtract_h(const F24x2 x)
{
	// extracts high value from vector 24-bit fractional and converts it
	// to double

	return Int32ToDouble(F24x2ToIntExtract_h(x));
}

ALWAYS_INLINE double F24x2ToDoubleExtract_l(const F24x2 x)
{
	// extracts low value from vector 24-bit fractional and converts it
	// to double

	return Int32ToDouble(F24x2ToIntExtract_l(x));
}

#endif /* EXTERNAL_AND_INTERNAL_TYPES_CONVERTERS */
