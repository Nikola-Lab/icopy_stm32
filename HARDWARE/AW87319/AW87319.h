#ifndef __AW87319_H
#define __AW87319_H
#include "myiic.h"   

u8		AW87319_ReadOneByte(u8 ReadAddr);								//指定地址读取一个字节
void	AW87319_WriteOneByte(u8 WriteAddr, u8 DataToWrite);				//指定地址写入一个字节
void	AW87319_WriteLenByte(u8 WriteAddr, u32 DataToWrite, u8 Len);	//指定地址开始写入指定长度的数据
u32		AW87319_ReadLenByte(u8 ReadAddr, u8 Len);						//指定地址开始读取指定长度数据
void	AW87319_Write(u8 WriteAddr, u8 *pBuffer, u16 NumToWrite);		//从指定地址开始写入指定长度的数据
void	AW87319_Read(u8 ReadAddr, u8 *pBuffer, u16 NumToRead);   		//从指定地址开始读出指定长度的数据

u8		AW87319_Check(void);											//检查器件
void	AW87319_Init(void);												//初始化IIC
#endif
