#ifndef __SPI_H
#define __SPI_H			  	 
#include "sys.h"
#include "stm32f10x_spi.h"

// SPI总线速度设置 
#define SPI_SPEED_2   		0
#define SPI_SPEED_4   		1
#define SPI_SPEED_8   		2
#define SPI_SPEED_16  		3
#define SPI_SPEED_32 		4
#define SPI_SPEED_64 		5
#define SPI_SPEED_128 		6
#define SPI_SPEED_256 		7
void SPI1_Init(void);   
u8 SPI1_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
void SPI1_SetSpeed(u8 SpeedSet);
#endif




