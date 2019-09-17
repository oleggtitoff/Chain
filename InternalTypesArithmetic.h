/*
 * InternalTypesArithmetic.h
 *
 *  Created on: Sep 3, 2019
 *      Author: Intern_2
 */

#ifndef INTERNAL_TYPES_ARITHMETIC
#define INTERNAL_TYPES_ARITHMETIC

#include "InternalTypesDefinitions.h"
#include "PredicatesOperations.h"
#include "ExternalAndInternalTypesConverters.h"
#include "Tables.h"

#define DIV_PRECISION 5		//be careful with too small values

//
// INTERNAL TYPES LOGICAL OPERATIONS
//
ALWAYS_INLINE F24x2 F24x2NOT(const F24x2 x)
{
	// implements bitwise NOT logical operation on x

	return AE_NOTP48(x);
}

ALWAYS_INLINE F24x2 F24x2AND(const F24x2 x, const F24x2 y)
{
	// implements bitwise AND logical operation on x and y

	return AE_ANDP48(x, y);
}

ALWAYS_INLINE F24x2 F24x2OR(const F24x2 x, const F24x2 y)
{
	// implements bitwise OR logical operation on x and y

	return AE_ORP48(x, y);
}

ALWAYS_INLINE F24x2 F24x2XOR(const F24x2 x, const F24x2 y)
{
	// implements bitwise XOR logical operation on x and y

	return AE_XORP48(x, y);
}

ALWAYS_INLINE F56x2 F56x2AND(const F56x2 x, const F56x2 y)
{
	// implements bitwise AND logical operation on x and y

	F56x2 res;
	res.h = AE_ANDQ56(x.h, y.h);
	res.l = AE_ANDQ56(x.l, y.l);
	return res;
}

//
// MIN/MAX OPERATIONS
//
ALWAYS_INLINE F24x2 F24x2Max(const F24x2 x, const F24x2 y)
{
	// returns maximum of two numbers

	return AE_MAXP24S(x, y);
}

ALWAYS_INLINE F24x2 F24x2Min(const F24x2 x, const F24x2 y)
{
	// returns minimum of two numbers

	return AE_MINP24S(x, y);
}

ALWAYS_INLINE F24x2 F24x2MaxAbs(const F24x2 x, const F24x2 y)
{
	// returns maximum absolute value of two numbers

	return AE_MAXABSSP24S(x, y);
}

ALWAYS_INLINE F24x2 F24x2MinAbs(const F24x2 x, const F24x2 y)
{
	// returns minimum absolute value of two numbers

	return AE_MINABSSP24S(x, y);
}

//
// INTERNAL TYPES ARITHMETIC
//
ALWAYS_INLINE F24x2 F24x2Zero()
{
	// returns zero in vector 24-bit fractional representation

	return AE_ZEROP48();
}

ALWAYS_INLINE F56x2 F56x2Zero()
{
	// returns zero in vector 56-bit fractional representation

	F56x2 res;
	res.h = AE_ZEROQ56();
	res.l = AE_ZEROQ56();
	return res;
}

ALWAYS_INLINE F56 F56LeftShiftL(F56 x, const uint8_t shift)
{
	// returns x value logically shifted to the left by "shift" bits

	return AE_SLLAQ56(x, shift);
}

ALWAYS_INLINE F56 F56LeftShiftAS(const F56 x, const uint8_t shift)
{
	// returns x value arithmetically shifted to the left by "shift" bits

	return AE_SLLASQ56S(x, shift);
}

ALWAYS_INLINE F24x2 F24x2LeftShiftAS(const F24x2 x, const uint8_t shift)
{
	// returns x value arithmetically shifted to the left by "shift" bits

	WAE_SAR(shift);
	return AE_SLLSSP24S(x);
}

ALWAYS_INLINE F24x2 F24x2LeftShiftAS_Apart(const F24x2 x, const uint8_t shiftH,
														  const uint8_t shiftL)
{
	// returns vector x value separately shifted to the left
	// by "shiftH" and "shiftL" bits

	return AE_SELP24_LL(
						F24x2LeftShiftAS(AE_SELP24_HH(x, x), shiftH),
						F24x2LeftShiftAS(AE_SELP24_LL(x, x), shiftL));
}

ALWAYS_INLINE F56x2 F56x2LeftShiftL(F56x2 x, const uint8_t shift)
{
	// returns x value logically shifted to the left by "shift" bits

	x.h = F56LeftShiftL(x.h, shift);
	x.l = F56LeftShiftL(x.l, shift);
	return x;
}

ALWAYS_INLINE F56x2 F56x2LeftShiftAS(F56x2 x, const uint8_t shift)
{
	// returns x value arithmetically shifted to the left by "shift" bits

	x.h = F56LeftShiftAS(x.h, shift);
	x.l = F56LeftShiftAS(x.l, shift);
	return x;
}

ALWAYS_INLINE F56 F56RightShiftA(const F56 x, const uint8_t shift)
{
	// returns x value arithmetically shifted to the right by "shift" bits

	return AE_SRAAQ56(x, shift);
}

ALWAYS_INLINE F56 F56RightShiftL(const F56 x, const uint8_t shift)
{
	// returns x value logically shifted to the right by "shift" bits

	return AE_SRLAQ56(x, shift);
}

ALWAYS_INLINE F24x2 F24x2RightShiftA(const F24x2 x, const uint8_t shift)
{
	// returns x value arithmetically shifted to the right by "shift" bits

	WAE_SAR(shift);
	return AE_SRASP24(x);
}

ALWAYS_INLINE F24x2 F24x2RightShiftA_Apart(const F24x2 x, const uint8_t shiftH,
														  const uint8_t shiftL)
{
	// returns vector x value separately shifted to the right
	// by "shiftH" and "shiftL" bits

	return AE_SELP24_LL(
						F24x2RightShiftA(AE_SELP24_HH(x, x), shiftH),
						F24x2RightShiftA(AE_SELP24_LL(x, x), shiftL));
}

ALWAYS_INLINE F56x2 F56x2RightShiftL(F56x2 x, const uint8_t shift)
{
	// returns x value logically shifted to the right by "shift" bits

	x.h = F56RightShiftL(x.h, shift);
	x.l = F56RightShiftL(x.l, shift);
	return x;
}

ALWAYS_INLINE F56x2 F56x2RightShiftA(F56x2 x, const uint8_t shift)
{
	// returns x value arithmetically shifted to the right by "shift" bits

	x.h = F56RightShiftA(x.h, shift);
	x.l = F56RightShiftA(x.l, shift);
	return x;
}

ALWAYS_INLINE F24x2 F24x2Abs(const F24x2 x)
{
	// returns the absolute value of x

	return AE_ABSSP24S(x);
}

ALWAYS_INLINE F56x2 F64x2Abs(F56x2 x)
{
	// returns the absolute value of x

	x.h = AE_ABSSQ56S(x.h);
	x.l = AE_ABSSQ56S(x.l);
	return x;
}

ALWAYS_INLINE F24x2 F24x2Add(const F24x2 x, const F24x2 y)
{
	// returns the sum of x and y

	return AE_ADDSP24S(x, y);
}

ALWAYS_INLINE F56 F56Add(const F56 x, const F56 y)
{
	return AE_ADDSQ56S(x, y);
}

ALWAYS_INLINE F56x2 F56x2Add(const F56x2 x, const F56x2 y)
{
	F56x2 res;
	res.h = F56Add(x.h, y.h);
	res.l = F56Add(x.l, y.l);
	return res;
}

ALWAYS_INLINE F24x2 F24x2Sub(const F24x2 x, const F24x2 y)
{
	// returns the difference of x and y

	return AE_SUBSP24S(x, y);
}

ALWAYS_INLINE F56x2 F24x2MacF56x2(F56x2 acc, const F24x2 x, const F24x2 y)
{
	// multiplies x by y and adds acc

	AE_MULAFS56P24S_HH(acc.h, x, y);
	AE_MULAFS56P24S_LL(acc.l, x, y);
	return acc;
}

ALWAYS_INLINE F56x2 F24x2MSubF56x2(F56x2 acc, const F24x2 x, const F24x2 y)
{
	// subtracts the product of x and y from the acc

	AE_MULSFS56P24S_LL(acc.l, x, y);
	AE_MULSFS56P24S_LL(acc.h, x, y);
	return acc;
}

ALWAYS_INLINE F24x2 F24x2Mul(const F24x2 x, const F24x2 y)
{
	// multiplies x by y and returns rounded to F24x2 result
	// uses Mac function with zero accumulator

	F56x2 mulRes = F24x2MacF56x2(F56x2Zero(), x, y);
	F24x2 round1 = AE_ROUNDSP24Q48SYM(mulRes.h);
	F24x2 round2 = AE_ROUNDSP24Q48SYM(mulRes.l);
	return AE_SELP24_HH(round1, round2);
}

ALWAYS_INLINE F24x2 F24x2Div(F24x2 x, F24x2 y)
{
	// divides x by y

	// coeffs
	F24x2 c1 = IntToF24x2Set(0x43e0f83e);		// Q4.20
	F24x2 c2 = IntToF24x2Set(0xa2e8ba2f);		// Q4.20
	F24x2 c3 = IntToF24x2Set(0x295fad40);		// Q4.20

	F24x2 r;			// Q4.20
	F24x2 e;			// Q4.20
	F24x2 k;			// Q4.20
	F24x2 res;			// Q1.23

	int8_t i;
	Boolx2 isCalculated   = (Boolx2)0;
	// result sign definition
	Boolx2 resIsNegative  = F24x2LessThan(F24x2XOR(x, y), F24x2Zero());

	// get absolute values of x and y
	x = F24x2Abs(x);
	y = F24x2Abs(y);

	// special cases handling
	// if x is zero, res = 0:
	Boolx2 xIsZero = F24x2Equal(x, F24x2Zero());
	F24x2MovIfTrue(&res, F24x2Zero(), xIsZero);
	isCalculated   = xIsZero;

	// if y is zero, res = MAX:
	Boolx2 yIsZero = F24x2Equal(y, F24x2Zero());
	F24x2MovIfTrue(&res, IntToF24x2Set(INT32_MAX), yIsZero);
	isCalculated   = Boolx2OR(isCalculated, yIsZero);

	// if x > y, x = y
	Boolx2 xIsBiggerThanY = F24x2LessThan(y, x);
	F24x2MovIfTrue(&x, y, xIsBiggerThanY);

	// convert y high and low parts to yh and yl in Q9.47
	F56 yh = AE_CVTQ48P24S_H(y);
	F56 yl = AE_CVTQ48P24S_L(y);

	// normalization to [0.5, 1.0]
	int8_t clsH = XT_SUB(AE_NSAQ56S(yh), 8);
	int8_t clsL = XT_SUB(AE_NSAQ56S(yl), 8);
	x = F24x2LeftShiftAS_Apart(x, clsH, clsL);
	y = F24x2LeftShiftAS_Apart(y, clsH, clsL);

	// precalculation
	F24x2 mulRes1 = F24x2Mul(y, c3);
	F24x2 addRes1 = F24x2Add(c2, mulRes1);
	F24x2 mulRes2 = F24x2Mul(y, addRes1);
	r = F24x2Add(c1, mulRes2);

	// loop calculation
	for (i = 0; i < 2; i++)
	{
		mulRes1 = F24x2Mul(y, r);
		e = F24x2Sub(IntToF24x2Set(0x0fffffff), mulRes1);

		mulRes1 = F24x2Mul(r, e);
		k = F24x2LeftShiftAS(mulRes1, 3);

		mulRes1 = F24x2Mul(k, e);
		mulRes1 = F24x2LeftShiftAS(mulRes1, 3);
		addRes1 = F24x2Add(k, mulRes1);
		r = F24x2Add(r, addRes1);
	}

	// final multiplication
	mulRes1 = F24x2Mul(x, r);
	F24x2MovIfTrue(&res, F24x2LeftShiftAS(mulRes1, 3), Boolx2NOT(isCalculated));

	// if result must be negative
	F24x2MovIfTrue(&res, F24x2Sub(F24x2Zero(), res), resIsNegative);

	return res;
}

ALWAYS_INLINE F24x2 F24x2InterpolL(const F24x2 x, const F24x2 y, const F24x2 alpha)
{
	// linear interpolation of two numbers by coefficient alpha (alpha = [0, 1))
	// if alpha = 0, function returns x
	// if alpha = 1, function returns y

	F24x2 xValue = F24x2Mul(x, F24x2Sub(IntToF24x2Set(0x7fffff00), alpha));
	F24x2 yValue = F24x2Mul(y, alpha);
	F24x2 res = F24x2Add(xValue, yValue);

	return res;
}

ALWAYS_INLINE F24x2 F24x2Log2(F24x2 x)
{
	// returns log2 of x
	// Input/Output in Q5.19

	F24x2 res = F24x2Zero();

	// If x == 0, res = INT32_MIN:
	Boolx2 isZero 		= F24x2Equal(x, F24x2Zero());
	Boolx2 isCalculated = isZero;
	F24x2MovIfTrue(&res, IntToF24x2Set(INT32_MIN), isZero);

	// x = absolute value of x:
	x = F24x2Abs(x);

	// convert x high and low parts to xh and xl in Q9.47
	F56 xh = AE_CVTQ48P24S_H(x);
	F56 xl = AE_CVTQ48P24S_L(x);

	// Amount to normalize:
	int16_t clsH = XT_SUB(AE_NSAQ56S(xh), 8);
	int16_t clsL = XT_SUB(AE_NSAQ56S(xl), 8);
	F24x2 cls 	 = IntToF24x2Join(clsH << 8, clsL << 8);

	// If x is bigger, than 1.0:
	Boolx2 isBigger  = Boolx2AND(F24x2LessThan(cls, IntToF24x2Set(4 << 8)),
								 Boolx2NOT(isCalculated));
	// If x is smaller, than 0.5:
	Boolx2 isSmaller = Boolx2AND(F24x2LessThan(IntToF24x2Set(4 << 8), cls),
								 Boolx2NOT(isCalculated));

	// Normalization:
	F24x2MovIfTrue(&x, F24x2RightShiftA_Apart(x, XT_SUB(4, clsH), XT_SUB(4, clsL)), isBigger);
	F24x2MovIfTrue(&res, F24x2LeftShiftAS(F24x2Sub(IntToF24x2Set(4 << 8), cls), 19), isBigger);
	F24x2MovIfTrue(&x, F24x2LeftShiftAS_Apart(x, XT_SUB(clsH, 4), XT_SUB(clsL, 4)), isSmaller);
	F24x2MovIfTrue(&res,
				   F24x2LeftShiftAS(F24x2Sub(F24x2Zero(),
						   	   	   	   	   	 F24x2Sub(cls, IntToF24x2Set(4 << 8))), 19),
				   isSmaller);

	// Here 0x4000000 is min (first) value in log2InputsTable
	// and 0x80000 is the step between values
	F24x2 alpha 	= F24x2LeftShiftAS(F24x2AND(x, IntToF24x2Set(0x7ffff)), 12);		// Q31
	F24x2 lowIndex 	= F24x2RightShiftA(F24x2Sub(x, IntToF24x2Set(0x4000000)), 11);
	F24x2 highIndex = F24x2Add(lowIndex, IntToF24x2Set(0x1 << 8));

	// Get values from table:
	F24x2 lowValue 	= IntToF24x2Join(
								log2OutputsTable[(int)F24x2ToIntExtract_h(lowIndex) >> 8],
								log2OutputsTable[(int)F24x2ToIntExtract_l(lowIndex) >> 8]);
	F24x2 highValue = IntToF24x2Join(
								log2OutputsTable[(int)F24x2ToIntExtract_h(highIndex) >> 8],
								log2OutputsTable[(int)F24x2ToIntExtract_l(highIndex) >> 8]);

	// Linear interpolation
	F24x2 tableRes = F24x2InterpolL(lowValue, highValue, alpha);
	F24x2MovIfTrue(&res, F24x2Add(res, tableRes), Boolx2NOT(isCalculated));
	return res;
}

ALWAYS_INLINE F24x2 F24x2PowOf2(F24x2 x)
{
	// returns 2 in pow of x
	// Input/Output in Q5.19

	F24x2 res = IntToF24x2Set(0x8000000);
	F24x2 mask = IntToF24x2Set(0x78000000);
	Boolx2 isNegative = F24x2LessThan(x, F24x2Zero());
	F24x2 absX = F24x2Abs(x);
	F24x2 count = F24x2AND(absX, mask);
	F24x2 countShifted = F24x2RightShiftA(count, 19);
	F24x2 tmpX = x;
	F24x2 tmpRes = res;

	x = F24x2Sub(tmpX, count);
	res = F24x2LeftShiftAS_Apart(tmpRes, (int)F24x2ToIntExtract_h(countShifted) >> 8,
			   	   	   	   	     (int)F24x2ToIntExtract_l(countShifted) >> 8);

	F24x2MovIfTrue(&x, F24x2Add(tmpX, count), isNegative);
	F24x2MovIfTrue(&res,
			F24x2RightShiftA_Apart(tmpRes, (int)F24x2ToIntExtract_h(countShifted) >> 8,
								   (int)F24x2ToIntExtract_l(countShifted) >> 8),
			isNegative);

	F24x2 alphaMask = F24x2AND(x, IntToF24x2Set(0xfffff));
	F24x2 alpha = F24x2LeftShiftAS(alphaMask, 11);
	F24x2 lowIndex = F24x2RightShiftA(x, 12);
	lowIndex = F24x2Add(lowIndex, IntToF24x2Set(128 << 8));
	F24x2 highIndex = F24x2Add(lowIndex, IntToF24x2Set(0x1 << 8));

	// Get values from table:
	F24x2 lowValue = IntToF24x2Join(
								powOf2OutputsTable[(int)F24x2ToIntExtract_h(lowIndex) >> 8],
								powOf2OutputsTable[(int)F24x2ToIntExtract_l(lowIndex) >> 8]);
	F24x2 highValue = IntToF24x2Join(
								powOf2OutputsTable[(int)F24x2ToIntExtract_h(highIndex) >> 8],
								powOf2OutputsTable[(int)F24x2ToIntExtract_l(highIndex) >> 8]);

	// Linear interpolation
	F24x2 tableRes = F24x2InterpolL(lowValue, highValue, alpha);
	return F24x2LeftShiftAS(F24x2Mul(res, tableRes), 4);
}

ALWAYS_INLINE F24x2 F24x2Pow(F24x2 x, F24x2 y)
{
	// returns x in pow of y
	// Input/Output in Q5.19

	F24x2 log2Res = F24x2Log2(x);
	F24x2 mulRes = F24x2Mul(y, log2Res);
	F24x2 shiftRes = F24x2LeftShiftAS(mulRes, 4);
	F24x2 pow2Res = F24x2PowOf2(shiftRes);

	return pow2Res;
}

#endif /* INTERNAL_TYPES_ARITHMETIC */
