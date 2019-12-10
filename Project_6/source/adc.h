/*
 * filename: adc.h
 *
 * Created By: Mark Hinkle
 * Date Created: 12/05/2019
 */

#ifndef _ADC_H
#define _ADC_H

#include <stdint.h>

#define CHANNEL_GROUP_0		0U
#define CHANNEL_3 			3U
#define BEGIN_CONVERSION	0x5555	//not sure about this

/* ADC buffer full flag */
extern uint8_t adcBufFull;

void adcInit(void);
void adcBeginConversion(void);
void ADC0_IRQHandler(void);


#endif /* _ADC_H */
