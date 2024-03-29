/*
 * filename: dma.c
 *
 * Created by: Mark Hinkle
 * Date Created: 12/05/2019
 */
#include "dma.h"
#include "fsl_dma.h"
#include "circular_buffer.h"
//#include "fsl_dmamux.h"


extern CircularBuffer_t * ADC_Buf;
extern CircularBuffer_t * DSP_Buf;

/*
 * @brief	Initializes DMA module and channel 0 for use in application
 */
void dmaInit(void)
{
	DMA_Init(DMA0);

	dma_transfer_config_t  dma_config;

	uint32_t dest_addr = (uint32_t) (DSP_Buf->buffer_start)  & 0x200FFFFF;
	uint32_t src_addr = (uint32_t) (ADC_Buf->buffer_start)  & 0x200FFFFF;

	dma_config.destAddr 			= dest_addr;
	dma_config.destSize 			= kDMA_Transfersize16bits;
	dma_config.srcAddr 			= src_addr;
	dma_config.srcSize 			= kDMA_Transfersize16bits;
	dma_config.enableDestIncrement = true;
	dma_config.enableSrcIncrement 	= true;
	dma_config.transferSize 		= DMA_128_BYTES;

	DMA_SetTransferConfig(DMA0, DMA_CHANNEL_0, &dma_config);

	DMA0->DMA[DMA_CHANNEL_0].DCR |= DMA_DCR_EINT(1); // | DMA_DCR_DMOD(DMA_128B_SZ) | DMA_DCR_SMOD(DMA_128B_SZ);

	NVIC_EnableIRQ(DMA0_IRQn);
}

/*
 * @brief	Starts DMA transfer
 */
void dmaBeginTransfer()
{
	DMA0->DMA[DMA_CHANNEL_0].DCR |= DMA_DCR_START(1);
}

void DMA0_IRQHandler(void)
{
	__disable_irq();
	//if transfer done
	if(DMA0->DMA[DMA_CHANNEL_0].DSR_BCR & DMA_DSR_BCR_DONE(1))
	{
		dmaDone = 1;
		CircBufReset(ADC_Buf);

		DMA0->DMA[DMA_CHANNEL_0].DSR_BCR |= DMA_DSR_BCR_DONE(1); //clear flag
	}
	__enable_irq();
}
