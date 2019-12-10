/*
 * filename: adc.c
 *
 * Created by: Mark Hinkle
 * Date Created: 12/05.2019
 */
#include "adc.h"
#include "fsl_adc16.h"
#include "circular_buffer.h"
#include "dma.h"

extern CircularBuffer_t * ADC_Buf;
extern CircularBuffer_t * DSP_Buf;
extern uint8_t error;

static CircBufferReturn_t ret;

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
	{
		error = 0x1;
	}

	flags = ADC16_GetChannelStatusFlags(ADC0, CHANNEL_GROUP_0);
	if(flags == kADC16_ChannelConversionDoneFlag)
	{
		//get data
		val = ADC0->R[0] & 0xFFF;
		ret = CircBufAdd(ADC_Buf, val);
		if(ret == BUF_FULL)
		{
			adcBufFull = 1;
			//trigger dma transfer
//			dmaBeginTransfer();
			NVIC_DisableIRQ(ADC0_IRQn);
		}
	}
}
