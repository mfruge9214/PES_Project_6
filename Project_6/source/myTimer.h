/*
 * File: myTimer.h
 * Authors: Mike Fruge and Mark Hinkle
 */

#include "FreeRTOS.h"
#include "timers.h"


extern TimerHandle_t myTimer;

void myTimerCreate(void);
void myTimerStart(void);
void myTimerCallback( TimerHandle_t timer);
