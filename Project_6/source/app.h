#include "FreeRTOS.h"
#include "task.h"
#include <stdbool.h>


#define NUM_SAMPLES		64.0

/* T A S K S */
void prv_InitModules(void *prvParameters);
void prv_GenerateDACSineWave(void *prvParameters);
void prv_ReadADC(void *prvParameters);
void prv_ProcessData(void * prvParameters);
void prv_ErrorHandler(void * prvParameters);

