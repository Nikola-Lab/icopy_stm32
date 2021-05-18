#ifndef __DMA_H
#define	__DMA_H	   
#include "sys.h"
#include "stm32f10x.h"		    

void DMA_8L_MemoryInc(DMA_Channel_TypeDef*DMA_CHx, u32 cpar, u32 cmar, u16 cndtr);//配置DMA1_CHx
void DMA_16L_NoMemoryInc(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);
void DMA_8L_NoMemoryInc(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);
void DMA_16L_MemoryInc(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);
void SPIDMA_Config(void);
void SPI1_Read_DMA(u8* buf, u16 num);

void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx);//使能DMA1_CHx
static void DMA_NVIC_Configuration(void);	   
#endif







