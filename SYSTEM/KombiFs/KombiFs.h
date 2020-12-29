#ifndef _KFS_H
#define _KFS_H
#include "stm32f10x.h"
#include "ICOPYX_BSP.h"

#define RESBaseaddr		0x0
#define BOXSTARTbyte	0x61
#define BOXSTOPbyte		0x62
//KFS分区表databox起止位，1个字节描述

#define RESID			0x63
//KFS文件id，1个字节描述
#define RESADDR			0x64
//KFS文件存储地址，4个字节描述

#define PICW			0x71
#define PICH			0x72
#define PICX			0x73
#define PICY			0x74
#define PICSIZE			0x75
//实例化databox图片参数记录标志位，1个字节描述

#define FONTSIZE		0x76
#define FONTX			0x77
#define FONTY			0x78
//实例化databox图片参数记录标志位，1个字节描述

extern u8 FILEPARAS[];

//实例化数据结构描述
/*实例化数据结构采用键值方式记录目标参数
 *图片数据参数有图片尺寸（宽和高）、图片数据尺寸、图片座标（x，y）
 *字库数据有字库数据尺寸、字库字体尺寸（宽和高）
 *参数表结构：由多组datacase直接拼接组成，datacase格式如下：
 *参数标志	数据长度(字节单位)	数据本体					......		
 *(0x00)	(0xXX)				XX(todec)x(0x00)		......		
 **/


//KFS数据结构描述
/*KFS采用较为简单的分区表方式存储文件
 *分区表在目标存储器内采用固定地址的方式存储，其记录了目标数据的相关参数
 *所有文件都拥有唯一的编号进行定位。
 *分区表结构：由多组databox直接拼接组成，databox格式如下：
 *起始位			id标志	id		数据地址标志		数据地址		|	数据段		|	......		结束位		(下一个databox开始)
 *BOXSTARTbyte	RESID	(0x00)	RESADDR			4x(0x00)	|	Nx(0x00)	|	......		BOXSTOPbyte	BOXSTARTbyte
 **/

u8 KFS_repair_fs();
//从文件系统里读出文件

u8 KFS_read_data(u32 ReadAddr, u32* lastaddr);
//分析数据包，lastaddr返回目标最后地址（1个u32）
//函数返回读到的文件id

u32 KFS_read_case(u8 id, u32 addr);
//解析数据块内容,返回值为所有数据结束后的下一个地址

void KFS_Read(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);		//读取hal接口
void KFS_Write(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);		//写入hal接口
void KFS_SetCur(u32 ReadAddr);										//设置光标所在地址
u16 KFS_ReadCur();													//读取光标所在地址
u8 KFS_ReadByte();													//读取一个byte

#endif
