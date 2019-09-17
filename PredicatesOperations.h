/*
 * PredicatesOperations.h
 *
 *  Created on: Sep 4, 2019
 *      Author: Intern_2
 */

#ifndef PREDICATES_OPERATIONS
#define PREDICATES_OPERATIONS

#include "InternalTypesDefinitions.h"

//
// LOGICAL OPERATIONS
//
ALWAYS_INLINE Boolx2 Boolx2NOT(const Boolx2 x)
{
	// implements bitwise NOT logical operation on x

	return (Boolx2)~(int)x;
}

ALWAYS_INLINE Boolx2 Boolx2AND(const Boolx2 x, const Boolx2 y)
{
	// implements bitwise AND logical operation on x and y

	return (Boolx2)XT_AND((int)x, (int)y);
}

ALWAYS_INLINE Boolx2 Boolx2OR(const Boolx2 x, const Boolx2 y)
{
	// implements bitwise OR logical operation on x and y

	return (Boolx2)XT_OR((int)x, (int)y);
}

ALWAYS_INLINE Boolx2 Boolx2XOR(const Boolx2 x, const Boolx2 y)
{
	// implements bitwise XOR logical operation on x and y

	return (Boolx2)XT_XOR((int)x, (int)y);
}

//
// COMPARSION OPERATIONS
//
ALWAYS_INLINE Boolx2 F24x2Equal(const F24x2 x, const F24x2 y)
{
	// element-wise equality

	return AE_EQP24(x, y);
}

ALWAYS_INLINE Boolx2 F24x2LessThan(const F24x2 x, const F24x2 y)
{
	// element-wise signed less-than comparison

	return AE_LTP24S(x, y);
}

ALWAYS_INLINE Boolx2 F24x2LessEqual(const F24x2 x, const F24x2 y)
{
	// element-wise signed less-than-or-equal comparison

	return AE_LEP24S(x, y);
}

//
// CONDITIONAL MOV OPERATIONS
//
ALWAYS_INLINE void F24x2MovIfTrue(F24x2 *inout, F24x2 x, Boolx2 a)
{
	// if a is set, copy the contents of x to inout

	AE_MOVTP24X2(*inout, x, a);
}

#endif /* PREDICATES_OPERATIONS */
