#ifndef __IP5306_H
#define __IP5306_H
#include "myiic.h"   

u8		IP5306_ReadOneByte(u8 ReadAddr);								//指定地址读取一个字节
void	IP5306_WriteOneByte(u8 WriteAddr, u8 DataToWrite);				//指定地址写入一个字节
void	IP5306_WriteLenByte(u8 WriteAddr, u32 DataToWrite, u8 Len);	//指定地址开始写入指定长度的数据
u32		IP5306_ReadLenByte(u8 ReadAddr, u8 Len);						//指定地址开始读取指定长度数据
void	IP5306_Write(u8 WriteAddr, u8 *pBuffer, u16 NumToWrite);		//从指定地址开始写入指定长度的数据
void	IP5306_Read(u8 ReadAddr, u8 *pBuffer, u16 NumToRead);   		//从指定地址开始读出指定长度的数据

u8		IP5306_Check(void);											//检查器件
void	IP5306_Init(void);												//初始化IIC
#endif
