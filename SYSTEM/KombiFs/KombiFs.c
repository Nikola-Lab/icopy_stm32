#include "KombiFs.h"
#include "w25qxx.h"


//目标存储器空间限制
u32 KFS_MAX_ADDR = (u32)0x00400000;
u32 KFS_Aim_Addr = (u32)0x00000000;
//1M存储器空间限制为80*1024*1024（字节）

//83886080（dec）字节地址为5000000（hex）
void KFS_Read(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
	W25QXX_Read(pBuffer,ReadAddr,NumByteToRead);
}
void KFS_Write(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
	W25QXX_Write(pBuffer, WriteAddr, NumByteToWrite);
}

void KFS_SetCur(u32 ReadAddr)										//设置光标所在地址
{
	KFS_Aim_Addr = ReadAddr;
}
u16 KFS_ReadCur()													//读取光标所在地址
{
	return KFS_Aim_Addr;
}
u8 KFS_ReadByte(u32 ReadAddr)										//读取一个byte并对地址自增
{
	u8 temp[2];
	KFS_Read(temp, KFS_Aim_Addr, 1);
	if (KFS_Aim_Addr >= KFS_MAX_ADDR)
	{
		return 0xff;
	}
	else
	{
		KFS_Aim_Addr += 1;
	}
	u8 data = temp[0];
	return data;
}									