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
	ADC16_GetDefaultConfig(&adc_config);
	ADC16_Init(ADC0, &adc_config);
	ADC0->SC2 &= ~ADC_SC2_ADTRG_MASK;	//enable software triggered conversions

	ADC16_DoAutoCalibration(ADC0);

	adc16_channel_config_t CH3_config;
	CH3_config.channelNumber = CHANNEL_3;
	CH3_config.enableDifferentialConversion = false;
	CH3_config.enableInterruptOnConversionCompleted = true;
	ADC16_SetChannelConfig(ADC0, CHANNEL_GROUP_0, &CH3_config);

	ADC16_EnableDMA(ADC0, false); //disable dma after conversion, we manually will when buffer is full

	NVIC_EnableIRQ(ADC0_IRQn);
}

void adcBeginConversion(void)
{
	ADC0->SC1[CHANNEL_GROUP_0] = BEGIN_CONVERSION;
}

void ADC0_IRQHandler(void)
{
	uint32_t flags, val;

	flags = ADC16_GetStatusFlags(ADC0);
	if(flags == kADC16_CalibrationFailedFlag)
		return; 		//not good

	flags = ADC16_GetChannelStatusFlags(ADC0, CHANNEL_GROUP_0);
	if(flags == kADC16_ChannelConversionDoneFlag)
	{
		//get data
		val = ADC0->R[0] & 0xFFF;
	}
}
