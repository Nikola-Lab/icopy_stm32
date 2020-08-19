#ifndef __SPI_H
#define __SPI_H			  	 
#include "sys.h"
#include "stm32f10x_spi.h"

void SPI1_Init(void);   
u8 SPI1_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
#endif




