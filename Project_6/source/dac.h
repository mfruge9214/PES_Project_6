/*
 * File: dac.h
 * Authors: Mike Fruge and Mark Hinkle
 */

/* Includes */
#include "app.h"

/* Defines */

#define NUM_SIN_VALS	50
#define PI				3.141592653589793
#define STEP			.1256637
#define VREF			3.300

typedef enum{
	DAC_SUCCESS,
	DAC_FAIL
}dac_ret_t;



/* Function Prototypes */

void DacSetLT(void);
dac_ret_t DacInit(void);
dac_ret_t DacSetVoltage(uint16_t regVal);
dac_ret_t DacIncrementAndSet(void);
