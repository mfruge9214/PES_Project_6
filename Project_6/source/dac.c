/*
 * File: dac.c
 * Authors: Mike Fruge and Mark Hinkle
 */

#include <math.h>
#include <stdint.h>
#include "MKL25Z4.h"
#include "dac.h"
#include "logger.h"
#include "gpio.h"
#include <stdio.h>

static uint16_t DAC_REG_VALS[NUM_SIN_VALS];		// Not const because values are calculated through code
static uint8_t dac_LT_idx;

/* F U N C T I O N S */

/*
 * @brief	Sets the DAC register values in the lookup table
 *
 * @usage	Each index in the DAC_REG_VALS array represents the register value that should be	\
 * 			loaded into the DACDAT register at each .1s increment
 *
 * 			Called in DACInit
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

		dacVal = (uint32_t) ((4096 * voltage / VREF) - 1);
		DAC_REG_VALS[i] = dacVal;
		mult += 1;
	}
	dac_LT_idx = 0;
	logString(LL_Normal, FN_DacSetLT, "Lookup table set with values");
}

/*
 * @brief Initializes the DAC module for use
 */
dac_ret_t DacInit(void)
{
	/* Set the values in the lookup table on initialization */
	DacSetLT();

	/* Configure DAC peripheral for use */

	// Route clocks to peripheral and GPIO ports

//	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;			Done in clk config?
	// Route pins
//	Done in Pin Mux
	// Configure peripheral registers

//	DAC0->C0 |= (DAC_C0_DACRFS_MASK);	// When enabled, this line selects Vref2 as the reference voltage

	/* This section applies when we go to do DMA
	 * 		Store the values in the Lookup Table into the DAC data buffer, and advance the read pointer
	 *
	 * DAC0->C0 |= DAC_C0_DACTRGSEL_MASK;		// Selects the software trigger to increment DAC buffer
	 *
	 * DAC0->C1 &= ~(DAC_C1_DACBFMD_MASK);		// Selects normal mode
	 *
	 * Need some way to set the read pointer equal to our lookup table buffer
	 * 		I thought it would be DAC_C2_DACBFRP but that is only one bit
	 * DAC0->C2 &= ~(DAC_C2_DACBFRP_MASK);		// Sets the read pointer to 0
	 *
	 *
	 *
	 * DAC0->C1 |= (DAC_C1_DACBUFEN_MASK;	// Enables the buffer
	 *
	 * When software trigger is selected and Buffer enabled, read pointer can be advanced by:
	 * 				DAC0->C0 |= DAC_C0_DACSWTRG;	// Advances read pointer by 1 word
	 *
	 * DAC0->C1 |= DAC_C1_DMAEN_MASK;		// Enables DMA
	 *
	 *
	 */

	DacSetVoltage(DAC_REG_VALS[dac_LT_idx]);

	// Enable
	DAC0->C0 |= (DAC_C0_DACEN_MASK);

	return DAC_SUCCESS;
}

/*
 * @brief	Sets the output voltage for the DAC after initialization
 *
 * @param[in]	uint16_t regVal			Value to write to DAC0->DAT
 *
 * @note	Output voltage is determined by
 * 		 Vout = Vin * (1 + dacVal) / 4096
		 dacVal = (4096 * Vout / Vin) - 1
 */

dac_ret_t DacSetVoltage(uint16_t regVal)
{
	uint8_t lowVal, hiVal;
	lowVal = regVal & 0xFF;		// Get lower 8 bits of data
	hiVal = (regVal & 0xF00) >> 8;
	DAC0->DAT->DATL = lowVal;
	DAC0->DAT->DATH = hiVal;

	return DAC_SUCCESS;
}

dac_ret_t DacIncrementAndSet()
{
#ifdef DMA
	// Increment by writing to the DACSWTRG register
#else
	// Chect to ensure index is within bounds
	if(dac_LT_idx < 0 || dac_LT_idx >= NUM_SIN_VALS)
	{
		// TODO: Adapt to error checking
		gpioRedLEDOn();
		return DAC_FAIL;
	}

	if(dac_LT_idx == NUM_SIN_VALS - 1)		// If index is at the end of the array
	{
		dac_LT_idx = -1;	// set to -1 since increment happens before indexing
	}
	uint16_t nextVal;
	nextVal = DAC_REG_VALS[++dac_LT_idx];	// Increment index, then obtain next value
	DacSetVoltage(nextVal);

	return DAC_SUCCESS;
#endif
}
