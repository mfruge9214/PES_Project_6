	/*
 * File: logger.c
 * Description: This file handles the logger functionality for the program
 * Authors: Mike Fruge and Mark Hinkle
 */

#include "logger.h"
#include "Systick.h"


/* Private Variable */
static bool LOG_ENABLE;
static LoggerLevel LOG_LEVEL;

static const char * LoggerLevelStrings[NUM_LEVELS] = {
		"Test: 	",
		"Debug:  ",
		"Status: "
};

static const char * FunctionNameStrings[NUM_FUNCTIONS] = {
		"main:				",
		"prv_InitModules:	",
		"prv_GenerateDACSineWave: ",
		"prv_ReadADC:		",
		"prv_ProcessData: 	",
		"prv_ErrorHandler:	",
		"DacIncrementAndSet: ",
		"DacSetLT: 			",
		"RedLEDOn: 			",
		"BlueLEDOn: 		",
		"GreenLEDOn: 		",
		"LEDsOff:			",
		"CircBufInit		",
		"CircBufRealloc		",
		"CircBufAdd			",
		"CircBufRemove		"
};

void logInit(LoggerLevel lvl)
{
	LOG_ENABLE = false;
	LOG_LEVEL = lvl;
}


void logEnable()
{
	if(LOG_ENABLE)
	{
		printf("Log already Enabled\n\r");
	}
	else
	{
		LOG_ENABLE = true;
		printf("Log Enabled\n\r");
	}

}


void logDisable()
{
	printf("Disabling Logger\n\r");
	if(LOG_ENABLE)
	{
		LOG_ENABLE = false;
	}
}


bool logStatus()
{
	return LOG_ENABLE;
}

void logString(LoggerLevel lvl, FunctionName fn, char* message)
{
//	char timeStamp[TIMESTAMP_LEN];
//	GetSysTickTime(timeStamp);
	if(!LOG_ENABLE)
	{
		return;
	}

	switch(LOG_LEVEL)
	{
	case LOGGER_LEVEL_NORMAL:
		if(lvl == LL_Normal)
		{
//			printf(timeStamp);
			printf(LoggerLevelStrings[lvl]);
			printf(FunctionNameStrings[fn]);
			printf(message);
			printf("\n\r");
		}
		break;

	case LOGGER_LEVEL_DEBUG:
		if((lvl == LL_Debug) || (lvl == LL_Normal))
		{
//			printf(timeStamp);
			printf(LoggerLevelStrings[lvl]);
			printf(FunctionNameStrings[fn]);
			printf(message);
			printf("\n\r");
		}
		break;

	case LOGGER_LEVEL_TEST:
//		printf(timeStamp);
		printf(LoggerLevelStrings[lvl]);
		printf(FunctionNameStrings[fn]);
		printf(message);
		printf("\n\r");
		break;

	default:
//		printf(timeStamp);
		printf("LOG_LEVEL is undefined \n\r");
		break;
	}
}


void logInteger(LoggerLevel lvl, FunctionName fn, uint32_t value)
{
//	char timeStamp[TIMESTAMP_LEN];
//	GetSysTickTime(timeStamp);
	/* Ensure Logger Enabled */
	if(!LOG_ENABLE)
	{
		return;
	}

	switch(LOG_LEVEL)
	{
	case LOGGER_LEVEL_NORMAL:
		if(lvl == LL_Normal)
		{
//			printf(timeStamp);
			printf(LoggerLevelStrings[lvl]);
			printf(FunctionNameStrings[fn]);
			printf("%u\n\r", value);
		}
		break;

	case LOGGER_LEVEL_DEBUG:
		if((lvl == LL_Debug) || (lvl == LL_Normal))
		{
//			printf(timeStamp);
			printf(LoggerLevelStrings[lvl]);
			printf(FunctionNameStrings[fn]);
			printf("%u\n\r", value);
		}
		break;

	case LOGGER_LEVEL_TEST:
//		printf(timeStamp);
		printf(LoggerLevelStrings[lvl]);
		printf(FunctionNameStrings[fn]);
		printf("%u\n\r", value);
		break;

	default:
//		printf(timeStamp);
		printf("LOG_LEVEL is undefined \n\r");
		break;
	}
}


void logFloat(LoggerLevel lvl, FunctionName fn, float value)
{
//	char timeStamp[TIMESTAMP_LEN];
//	GetSysTickTime(timeStamp);
	/* Ensure Logger Enabled */
	if(!LOG_ENABLE)
	{
		return;
	}

	switch(LOG_LEVEL)
	{
	case LOGGER_LEVEL_NORMAL:
		if(lvl == LL_Normal)
		{
//			printf(timeStamp);
			printf(LoggerLevelStrings[lvl]);
			printf(FunctionNameStrings[fn]);
			printf("%.3f\n\r", value);
		}
		break;

	case LOGGER_LEVEL_DEBUG:
		if((lvl == LL_Debug) || (lvl == LL_Normal))
		{
//			printf(timeStamp);
			printf(LoggerLevelStrings[lvl]);
			printf(FunctionNameStrings[fn]);
			printf("%.3f\n\r", value);
		}
		break;

	case LOGGER_LEVEL_TEST:
//		printf(timeStamp);
		printf(LoggerLevelStrings[lvl]);
		printf(FunctionNameStrings[fn]);
		printf("%.3f\n\r", value);
		break;

	default:
//		printf(timeStamp);
		printf("LOG_LEVEL is undefined \n\r");
		break;
	}
}


