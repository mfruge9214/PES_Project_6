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
//	DMA0->DMA.DAR;
	DMA_Init(DMA0);
}
