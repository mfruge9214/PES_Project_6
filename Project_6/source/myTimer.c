/*
 * File: myTimer.c
 * Authors: Mike Fruge and Mark Hinkle
 */

#include "myTimer.h"
#include "gpio.h"
#include "adc.h"
#include <stdbool.h>


/* Global Timer */
TimerHandle_t myTimer;

static bool lightOn;
/*
 * @brief	Creates instance of application timer
 */
void myTimerCreate(void){
	lightOn = false;
	TickType_t timerPeriod = pdMS_TO_TICKS(100);
	myTimer = xTimerCreate(".1s timer",
							timerPeriod,
							pdTRUE,
							(void *) 2,
							myTimerCallback);
//	return myTimer;
}

void myTimerStart(void)
{
	xTimerStart(myTimer, 0);		// Start timer, wait 0 ticks
}


void myTimerCallback( TimerHandle_t timer)
{
	if(!lightOn)
	{
		gpioBlueLEDOn();
		lightOn = true;
	}
	else
	{
		gpioLEDsOff();
		lightOn = false;
	}
	adcBeginConversion();
}
