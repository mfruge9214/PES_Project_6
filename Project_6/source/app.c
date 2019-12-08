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


#include "app.h"
#include "myTimer.h"
#include "dac.h"


/* A P P L I C A T I O N     M O D U L E S  */

TimerHandle_t * p1Timer;


/* E R R O R    V A R I A B L E */

uint8_t error;

/* T A S K S */

void prv_GenerateSineWave(void *prvParameters)
{
	bool lightOn = 0;
	TickType_t delayTime = pdMS_TO_TICKS(100);
	error = (uint8_t) DacInit();
	while(1)
	{
		vTaskDelay(delayTime);
		DacIncrementAndSet();
		if(!lightOn)
		{
			gpioGreenLEDOn();
			lightOn = 1;
		}
		else
		{
			gpioLEDsOff();
			lightOn = 0;
		}
	}
}
//void prv_TimerTask(void *prvParameters)
//{
//	myTimerCreate(p1Timer);
//	myTimerStart(p1Timer);
//}
