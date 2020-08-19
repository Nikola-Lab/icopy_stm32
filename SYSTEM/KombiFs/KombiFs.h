#ifndef KFS_H
#define KFS_H
#include "stm32f10x.h"

void KFS_Read(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);		//读取hal接口
void KFS_Write(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);		//写入hal接口
void KFS_SetCur(u32 ReadAddr);										//设置光标所在地址
u16 KFS_ReadCur();													//读取光标所在地址
u8 KFS_ReadBytes(u32 ReadAddr);									    //读取一个byte

#endif
