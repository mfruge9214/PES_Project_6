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

/* F R E E   R T O S   I N C L U D E S */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"


//static bool lightOn;

/* D E F I N I T I O N S */

/* Task priorities. */
#define genwave_PRIORITY (configMAX_PRIORITIES - 1)

//static void hello_task(void *pvParameters);
//
///*
// * @brief   Application entry point.
// */
//
//void myTimerCallback( TimerHandle_t timer)
//{
//	DacIncrementAndSet();
//	if(!lightOn)
//	{
//		gpioGreenLEDOn();
//		lightOn = true;
//	}
//	else
//	{
//		gpioLEDsOff();
//		lightOn = false;
//	}
//
//}


int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    /* Initialize application modules */
    logInit(LL_Debug);
    gpioInit();
//    DacInit();
//
//    /* Create timer module */
//    lightOn = false;
//    TickType_t timerPeriod = pdMS_TO_TICKS(100);
//    TimerHandle_t p1Timer =  xTimerCreate(".1s timer",
//    							timerPeriod,
//    							pdTRUE,
//    							(void *) 2,
//    							myTimerCallback);
//
//    xTimerStart(p1Timer, 0);		// Start timer, wait 0 ticks
//

    /* Create Tasks */

//    timer_task_handle = xTaskCreate(prv_TimerTask, "Timer Task", configTIMER_TASK_STACK_DEPTH, NULL, configTIMER_TASK_PRIORITY, NULL);
    xTaskCreate(prv_GenerateSineWave, "Generate_Sine_Wave", configTIMER_TASK_STACK_DEPTH, NULL, genwave_PRIORITY, NULL);
//    xTaskCreate(hello_task, "Hello_task", configMINIMAL_STACK_SIZE + 10, NULL, hello_task_PRIORITY, NULL);
    vTaskStartScheduler();
    while(1);
}


/*!
 * @brief Task responsible for printing of "Hello world." message.
 */
static void hello_task(void *pvParameters)
{
    for (;;)
    {
        PRINTF("Hello world.\r\n");
        vTaskSuspend(NULL);
    }
}
