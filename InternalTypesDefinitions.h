/*
 * InternalTypesDefinitions.h
 *
 *  Created on: Sep 2, 2019
 *      Author: Intern_2
 */

#ifndef INTERNAL_TYPES_DEFINITIONS
#define INTERNAL_TYPES_DEFINITIONS

#include <xtensa/tie/xt_hifi2.h>

#include <stdint.h>

//
// GLOBAL MACRO DEFINITIONS
//
#define ALWAYS_INLINE static inline __attribute__((always_inline))
#define NEVER_INLINE static __attribute__((noinline))


//
// INTERNAL TYPES DEFINITIONS
//
//Status	: status flag
typedef enum {
	statusOK = 0,
	statusError
} Status;


// Boolx2 	: vector boolean
typedef xtbool2 Boolx2;

// F56x1	: scalar 56-bit fractional
typedef ae_q56s F56;

// F24x2	: vector 24-bit fractional
typedef ae_p24x2f F24x2;

// F56x2	: vector 56-bit fractional
typedef struct {
	F56 h;
	F56 l;
} F56x2;

#endif /* INTERNAL_TYPES_DEFINITIONS */
