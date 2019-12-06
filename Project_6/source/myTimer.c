/*
 * File: myTimer.c
 * Authors: Mike Fruge and Mark Hinkle
 */

#include "myTimer.h"
#include "gpio.h"
#include "dac.h"
#include <stdbool.h>


/* Global Timer */


static bool lightOn;
/*
 * @brief	Creates instance of application timer
 */
void myTimerCreate(TimerHandle_t * ptTimer){
	lightOn = false;
	TickType_t timerPeriod = pdMS_TO_TICKS(100);
	(*ptTimer) = xTimerCreate(".1s timer",
							timerPeriod,
							pdTRUE,
							(void *) 2,
							myTimerCallback);
//	return myTimer;
}

void myTimerStart(TimerHandle_t * ptTimer)
{
	xTimerStart(*ptTimer, 0);		// Start timer, wait 0 ticks
}

/*
 * @brief Callback function that executes adc and gpio functions
 *
 * @note Callback function does not execute from an interrupt context
 */
void myTimerCallback( TimerHandle_t timer)
{
	DacIncrementAndSet();
	if(!lightOn)
	{
		gpioGreenLEDOn();
		lightOn = true;
	}
	else
	{
		gpioLEDsOff();
		lightOn = false;
	}

}
