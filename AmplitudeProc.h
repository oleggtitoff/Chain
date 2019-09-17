/*
 * AmplitudeProc.h
 *
 *  Created on: Sep 10, 2019
 *      Author: Intern_2
 */

#ifndef AMPLITUDE_PROC
#define AMPLITUDE_PROC

#include "InternalTypesArithmetic.h"
#include "InternalTypesConverters.h"

#define RING_BUFF_SIZE 		128
#define DATA_BUFF_SIZE 		512
#define CHANNELS 2

typedef struct {
	uint16_t currNum;
	F24x2 samples[RING_BUFF_SIZE];	// Q5.19
	F24x2 maxSample;				// Q5.19
} RingBuff;

typedef struct {
	F24x2 alphaAttack;			// Q1.23
	F24x2 alphaRelease;			// Q1.23
} EnvelopeCoeffs;

typedef struct {
	int8_t isActive;
	F24x2 threshold;			// Q5.19
} LimiterCoeffs;
typedef LimiterCoeffs NoiseGateCoeffs;

typedef struct {
	int8_t isActive;
	F24x2 threshold;			// Q5.19
	F24x2 C1;					// Q5.19
	F24x2 C2;					// Q5.19
	F24x2 alphaAttack;			// Q1.23
	F24x2 alphaRelease;			// Q1.23
} CompressorCoeffs;
typedef CompressorCoeffs ExpanderCoeffs;

typedef struct {
	EnvelopeCoeffs envelope;
	NoiseGateCoeffs noiseGate;
	LimiterCoeffs limiter;
	CompressorCoeffs compressor;
	ExpanderCoeffs expander;
} AmplitudeProcCoeffs;


typedef struct {
	F24x2 prevSample;			// Q5.19
} EnvelopeStates;

typedef struct {
	Boolx2 isWorked;
} NoiseGateStates;

typedef struct {
	Boolx2 isWorked;
	F24x2 prevGain;				// Q5.19
} CompressorStates;
typedef CompressorStates ExpanderStates;

typedef struct {
	RingBuff ringBuff;
	EnvelopeStates envelope;
	NoiseGateStates noiseGate;
	CompressorStates compressor;
	ExpanderStates expander;
} AmplitudeProcStates;


Status AmplitudeProcInit(AmplitudeProcCoeffs *coeffs, AmplitudeProcStates *states);
Status AmplitudeProcUpdateCoeffs(AmplitudeProcCoeffs *coeffs, AmplitudeProcCoeffs *newCoeffs);
F24x2 AmplitudeProc_Process(const AmplitudeProcCoeffs *coeffs, AmplitudeProcStates *states,
						   const F24x2 sample);

#endif /* AMPLITUDE_PROC */
