#ifndef __DMA_H
#define	__DMA_H	   
//#include "sys.h"
#include "bsp.h"
void DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar,u16 cndtr,u8 dir);
void DMA_Enable(DMA_Channel_TypeDef*DMA_CHx);   
#endif






























