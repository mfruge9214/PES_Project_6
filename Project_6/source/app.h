#include "FreeRTOS.h"
#include "task.h"
#include <stdbool.h>


/* T A S K S */
void prv_GenerateSineWave(void *prvParameters);
void prv_TimerTask(void *prvParameters);