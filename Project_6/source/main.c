/*
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    main.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include "dac.h"
#include "logger.h"
#include "gpio.h"
#include "Systick.h"
#include "myTimer.h"
#include "app.h"
#include "circular_buffer.h"
#include "adc.h"
#include "dma.h"

/* F R E E   R T O S   I N C L U D E S */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"


//static bool lightOn;

/* D E F I N I T I O N S */

/* Task priorities. */
#define initModules_PRIORITY 		(configMAX_PRIORITIES - 1)
#define errorHandler_PRIORITY		(initModules_PRIORITY - 1)
#define readADC_PRIORITY 			(initModules_PRIORITY - 2)
#define genwave_PRIORITY			(initModules_PRIORITY - 2)
#define processdata_PRIORITY		(initModules_PRIORITY - 2)

/******************** G L O B A L S *****************/

//extern uint8_t dmaDone;
//extern uint8_t adcBufFull;
//
//extern CircularBuffer_t * ADC_Buf;
//extern CircularBuffer_t * DSP_Buf;

extern TaskHandle_t initHandle, genWaveHandle, ADCHandle, processDataHandle, errorHandlerHandle;

/* MAIN */
int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    /* Create Tasks */

    xTaskCreate(prv_InitModules, "Application Module Init", (configMINIMAL_STACK_SIZE * 2), NULL, initModules_PRIORITY, &initHandle);
    xTaskCreate(prv_GenerateDACSineWave, "Generate_Sine_Wave", configMINIMAL_STACK_SIZE + 10, NULL, genwave_PRIORITY, &genWaveHandle);
    xTaskCreate(prv_ReadADC, "Read ADC", (configMINIMAL_STACK_SIZE * 2), NULL, readADC_PRIORITY, &ADCHandle);
    xTaskCreate(prv_ProcessData, "Process ADC Data", configMINIMAL_STACK_SIZE + 10, NULL, processdata_PRIORITY, &processDataHandle);
    xTaskCreate(prv_ErrorHandler, "Error Handling", configMINIMAL_STACK_SIZE, NULL, errorHandler_PRIORITY, &errorHandlerHandle);
    vTaskStartScheduler();
    while(1);
}

