/*
 * File: dac.c
 * Authors: Mike Fruge and Mark Hinkle
 */

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include "dac.h"

static uint32_t DAC_REG_VALS[NUM_SIN_VALS];

/*
 * @brief	Sets the DAC register values in the lookup table
 */

/*
 * Voltage determined by V=sin(2pit /5) + 2
 */
void DacSetLT(void){
	int i;
	double mult = 0;
	double voltage;
	double angle;
	double step;

	uint16_t dacVal;

	for(i = 0; i<NUM_SIN_VALS; i++)
	{
		step = mult/10;
		angle = ((double) (.4)) * PI * step;
		voltage = sin(angle) + (double) 2;

		/* Have actual output voltage, need to convert to register values */
		/* Vout = Vin * (1 + dacVal) / 4096 */
		/* dacVal = (4096 * Vout / Vin) - 1 */

		dacVal = (uint32_t) ((4096 * voltage / VREF) - 1);
		DAC_REG_VALS[i] = dacVal;
		mult += 1;
	}
}
