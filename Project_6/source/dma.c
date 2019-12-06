/*
 * filename: dma.c
 *
 * Created by: Mark Hinkle
 * Date Created: 12/05/2019
 */
#include "dma.h"
#include "fsl_dma.h"

void dmaInit(void)
{
//	DMA0->DMA->SAR = ADC_BUFFER_ADDR; //format 0x000x_xxxx
//	DMA0->DMA->DAR = DSP_BUFFER_ADDR; //format 0x000x_xxxx
	DMA0->DMA->DCR = DMA_DCR_EINT(1) | DMA_DCR_DMOD(DMA_128B_SZ) | DMA_DCR_SMOD(DMA_128B_SZ);

//	DMA0->DMA->DSR_BCR; //these are status flags and remaining bytes (read)

	DMA_Init(DMA0);
}
