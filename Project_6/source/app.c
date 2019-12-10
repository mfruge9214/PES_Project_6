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
#include "myTimer.h"
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
    logInit(LL_Debug);
    gpioInit();
    error = (uint8_t) DacInit();
    adcInit();
    dmaInit();

    if(error)
    {
    	xTaskNotifyGive(errorHandlerHandle);
    }

    vTaskDelete(NULL);
}

void prv_GenerateDACSineWave(void *prvParameters)
{
	bool lightOn = 0;
//	error = (uint8_t) DacInit();
	while(1)
	{
		vTaskDelay(delayTime);
		error = (uint8_t) DacIncrementAndSet();

		error = 1;

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

void prv_ProcessData(void * prvParameters)
{
	static uint32_t reg_min;
	static uint32_t reg_max;
	static uint32_t reg_sum;
	static uint32_t reg_avg;
	static uint32_t reg_val;

	static float avg = 0;
	static float min = 0;
	static float max = 0;
	static float sum = 0;
	static float std_dev = 0;

	int xferCnt = 0;
	int i;
	while(1)
	{
		if(dmaDone)
		{
			xferCnt++;
			NVIC_EnableIRQ(ADC0_IRQn);
			dmaDone = 0;
			dmaStopTime = xTaskGetTickCount();

			/* Reset variables that are not otherwise initialized */
			sum = 0;
			reg_sum = 0;


			reg_min = DSP_Buf->buffer_start[0];
			reg_max = reg_min;
			// Set max and min values to the first values in the buffer
			for(i=0; i<NUM_SAMPLES; i++)
			{
				reg_val = DSP_Buf->buffer_start[i];
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

			reg_avg = (reg_sum / NUM_SAMPLES);

			/* TODO	Finish calculating values */

			/* Code found at http://ecomputernotes.com/what-is-c/array/mean-and-standard-deviation */
			/* Calculate standard deviation */
//			sum += (reg_val - mean) * (reg_val - mean);
//			std_dev = sqrt(sum / NUM_SAMPLES);


		/* TODO After calculating values, report the run number and the time it took DMA to xfer */

		}
		if( xferCnt == 5)
		{
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


void prv_ErrorHandler(void * prvParameters)
{
//	TickType_t error_delay = pdMS_TO_TICKS(1000);
	while(1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if(xSemaphoreTake(ledSemaphore, (TickType_t) 100))
		{
			gpioRedLEDOn();
			/* TODO Add logger and throw error */


			while(1);
		}
//		vTaskDelay(error_delay);
	}
}

//void prv_TimerTask(void *prvParameters)
//{
//	myTimerCreate(p1Timer);
//	myTimerStart(p1Timer);
//}
