/*
 * File: App.c
 * Authors: Mike Fruge and Mark Hinkle
 *
 * This file contains all tasks used by the program, and also modules contained within the program scope
 */


/*
 * Things that will make this badass
 *  error checking task that uses queues to get errors from other tasks
 *  logging task
 */

/* F R E E   R T O S   I N C L U D E S */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "math.h"

#include "core_cm0plus.h"

#include "app.h"
#include "dac.h"
#include "gpio.h"
#include "circular_buffer.h"
#include "dma.h"
#include "adc.h"
#include "logger.h"


/* A P P L I C A T I O N    G L O B A L S */


		/* B U F F E R S */

CircularBuffer_t * ADC_Buf;
CircularBuffer_t * DSP_Buf;

		/* F L A G S */
uint8_t dmaDone = false;
uint8_t adcBufFull = false;

/* A P P L I C A T I O N     M O D U L E S  */

//TimerHandle_t * p1Timer;
SemaphoreHandle_t ledSemaphore = NULL;

/* E R R O R    V A R I A B L E */

uint8_t error;

TickType_t delayTime = pdMS_TO_TICKS(100);
TickType_t BlueOnTime = pdMS_TO_TICKS(500);


static TickType_t dmaStartTime;
static TickType_t dmaStopTime;

/* T A S K    H A N D L E S */
TaskHandle_t initHandle, genWaveHandle, ADCHandle, processDataHandle, errorHandlerHandle;

/* T A S K S */


/*
 * @brief	This task is a high priority task that executes before all other tasks to initialize the application modules
 *
 * @handle	initHandle
 *
 * @priority	initModules_PRIORITY
 *
 * @note 	This task terminates once complete
 */
void prv_InitModules(void *prvParameters)
{
	ledSemaphore = xSemaphoreCreateBinary();
	xSemaphoreGive(ledSemaphore);
    /* Initialize application modules */

    // Create buffers
	ADC_Buf = CircBufCreate();
	DSP_Buf = CircBufCreate();
	if(ADC_Buf == NULL || DSP_Buf == NULL)
	{
		error |= 0x1;
	}
	error = (uint8_t) CircBufInit(ADC_Buf, NUM_SAMPLES);
	error = (uint8_t) CircBufInit(DSP_Buf, NUM_SAMPLES);

	/* Initialize Peripherals */
    logInit(LL_Normal);
    logEnable();
    gpioInit();
    error = (uint8_t) DacInit();
    adcInit();
    dmaInit();

    logString(LL_Normal, FN_prv_InitModules, "Initialization Complete");

    if(error)
    {
    	xTaskNotifyGive(errorHandlerHandle);
    }

    vTaskDelete(NULL);
}

/*
 * @brief	This task increments and sets the DAC
 *
 * @handle	genWaveHandle
 *
 * @priority	genwave_PRIORITY
 *
 * @note 	This task cycles until terminated by another task
 */
void prv_GenerateDACSineWave(void *prvParameters)
{
	bool lightOn = 0;
	while(1)
	{
		vTaskDelay(delayTime);
		DacIncrementAndSet();
		error = (uint8_t) DacIncrementAndSet();

		if(error)
		{
			xTaskNotifyGive(errorHandlerHandle);
		}


		if(!lightOn)
		{
			if(xSemaphoreTake(ledSemaphore, (TickType_t) 10) == pdTRUE)  // == pdTrue)
			{
				gpioGreenLEDOn();
				xSemaphoreGive(ledSemaphore);
				lightOn = 1;
			}
		}
		else
		{
			if(xSemaphoreTake(ledSemaphore, (TickType_t) 10) == pdTRUE) //== pdTrue)
			{
				gpioLEDsOff();
				xSemaphoreGive(ledSemaphore);
				lightOn = 0;
			}
		}
	}
}

/*
 * @brief	This task reads the ADC and begins DMA transfers
 *
 * @handle	ADCHandle
 *
 * @priority	readADC_PRIORITY
 *
 * @note 	This task cycles until terminated by another task
 */
void prv_ReadADC(void * prvParameters)
{
	while(1)
	{
		if(error)
		{
			xTaskNotifyGive(errorHandlerHandle);
		}
		vTaskDelay(delayTime);
		adcBeginConversion();
		if(adcBufFull)
		{
		    logString(LL_Normal, FN_prv_ReadADC, "ADC Buf Full - Begin DMA");
			adcBufFull = 0;
			if(xSemaphoreTake(ledSemaphore, (TickType_t) 10) == pdTRUE) //== pdTrue)
			{
				gpioBlueLEDOn();
				vTaskDelay(BlueOnTime);
				gpioLEDsOff();
				xSemaphoreGive(ledSemaphore);
			}
			dmaStartTime = xTaskGetTickCount();
			dmaBeginTransfer();
		}
	}
}

/*
 * @brief	This task processes the data given to the DSP_Buffer
 *
 * @handle	processDataHandle
 *
 * @priority	processdata_PRIORITY
 *
 * @note 	This task terminates the other application tasks and then terminates itself
 */
void prv_ProcessData(void * prvParameters)
{
	static uint32_t reg_min;
	static uint32_t reg_max;
	static uint32_t reg_sum;
	static uint32_t reg_avg;
	static uint32_t reg_val;

	static float std_dev_sum = 0;
	static float temp = 0;
	static float Vavg = 0;
	static float Vmin = 0;
	static float Vmax = 0;
	static float Vsum = 0;
	static float Vstd_dev = 0;

	int xferCnt = 0;
	int i;
	while(1)
	{
		if(dmaDone)
		{
		    logString(LL_Normal, FN_prv_ProcessData, "DMA Complete - Time (ms):");

			xferCnt++;
			NVIC_EnableIRQ(ADC0_IRQn);
			dmaDone = 0;
			dmaStopTime = xTaskGetTickCount();

			uint32_t t = (uint32_t)(dmaStopTime - dmaStartTime);
			logInteger(LL_Normal, FN_prv_ProcessData, t);

			/* Reset variables that are not otherwise initialized */
			Vsum = 0;
			reg_sum = 0;


			reg_min = DSP_Buf->buffer_start[0];
			reg_max = reg_min;
			// Set max and min values to the first values in the buffer
			for(i=0; i<NUM_SAMPLES; i++)
			{
				if (i == 39)
				{
					int j;
				}
				reg_val = DSP_Buf->buffer_start[i];
//				DSP_Buf->head++;
				reg_sum += reg_val;


				if(reg_val < reg_min)
				{
					reg_min = reg_val;
				}

				if(reg_val > reg_max)
				{
					reg_max =reg_val;
				}
			}

			/* Average Calculations */
			reg_avg =( reg_sum / NUM_SAMPLES);
			Vsum = (float) (reg_sum);
			temp = (float) (Vsum / NUM_SAMPLES);
			Vavg = (temp/ (float) 4096) * (float) (3.30);

			/* Max Calculations */
			temp = (float) reg_max;
			Vmax = (temp/ (float) 4096) * (float) (3.30);

			/* Min Calculations */
			temp = (float) (reg_min);
			Vmin = (temp/ (float) 4096) * (float) (3.30);

			logString(LL_Normal, FN_prv_ProcessData, "Vmin - Vmax - Vavg");
			logFloat(LL_Normal, FN_prv_ProcessData, Vmin);
			logFloat(LL_Normal, FN_prv_ProcessData, Vmax);
			logFloat(LL_Normal, FN_prv_ProcessData, Vavg);

			/* Standard Deviation Calculations */

			/* Code found at http://ecomputernotes.com/what-is-c/array/mean-and-standard-deviation */
			std_dev_sum = 0;
			for(i = 0; i< NUM_SAMPLES; i++)
			{
				reg_val = DSP_Buf->buffer_start[i];
				temp = (float) (((float) reg_val)/(float) 4096) * ((float) 3.30);
				std_dev_sum += (temp - Vavg) * (temp - Vavg);
			}

			Vstd_dev = sqrt(std_dev_sum/NUM_SAMPLES);

			logString(LL_Normal, FN_prv_ProcessData, "Standard Deviation");
			logFloat(LL_Normal, FN_prv_ProcessData, Vstd_dev);

		/* TODO After calculating values, report the run number and the time it took DMA to xfer */

		}
		if( xferCnt == 5)
		{
		    logString(LL_Normal, FN_prv_ProcessData, "5 Cycles Complete - Program End");

			error = CircBufDestroy(ADC_Buf);
			error = CircBufDestroy(DSP_Buf);
			if(error)
			{
				xTaskNotifyGive(errorHandlerHandle);
			}
			vTaskDelete(genWaveHandle);		// Delete Genwave Task
			vTaskDelete(ADCHandle);			// Delete ADC task
		    vTaskDelete(NULL);		// Delete this task
		}
	}
}


/*
 * @brief	This task handles all errors generated by other tasks
 *
 * @handle	errorHandlerHandle
 *
 * @priority	errorHandler_PRIORITY
 *
 * @note 	This task is a high priority task that will wait for a notification to run from another task
 */
void prv_ErrorHandler(void * prvParameters)
{
	while(1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if(xSemaphoreTake(ledSemaphore, (TickType_t) 100))
		{
			gpioRedLEDOn();
			/* TODO Add logger and throw error */

			while(1);
		}
	}
}

