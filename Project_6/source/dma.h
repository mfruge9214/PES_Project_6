/*
 * filename: dma.h
 *
 * Created by: Mark Hinkle
 * Date Created: 12/05/2019
 */

#ifndef _DMA_H
#define _DMA_H

#include <stdint.h>

#define DMA_128B_SZ 	0x4		//0b0100
#define DMA_128_BYTES	128U
#define DMA_CHANNEL_0	0U		//not sure

extern uint8_t dmaDone;

void dmaInit(void);
void dmaBeginTransfer(void);
void DMA0_IRQHandler(void);


#endif /* _DMA_H */
