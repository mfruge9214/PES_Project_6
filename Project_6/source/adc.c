/*
 * filename: adc.c
 *
 * Created by: Mark Hinkle
 * Date Created: 12/05.2019
 */
#include "adc.h"
#include "fsl_adc16.h"

void adcInit(void)
{
	adc16_config_t adc_config;
	//needs works
	ADC16_Init(ADC0, &adc_config);
}
