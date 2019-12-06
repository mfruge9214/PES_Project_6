/*
 * File: myTimer.h
 * Authors: Mike Fruge and Mark Hinkle
 */

#include "FreeRTOS.h"
#include "timers.h"

void myTimerCreate(TimerHandle_t * ptTimer);
void myTimerStart(TimerHandle_t * ptTimer);
void myTimerCallback( TimerHandle_t timer);
