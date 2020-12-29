#include "KombiFs.h"
#include "w25qxx.h"

u8 FILEPARASPIC[] = { PICW, PICH, PICX, PICY, PICSIZE};
u8 FILEPARASFONT[] = { FONTSIZE, FONTX, FONTY };
//目标存储器空间限制
u32 KFS_MAX_ADDR = (u32)0x00100000;
u32 KFS_Aim_Addr = (u32)0x00000000;
//1M存储器空间限制为80*1024*1024（字节）

//83886080（dec）字节地址为5000000（hex）

u8 KFS_repair_fs()
{
	u32 tempaddr = 0x00000000;
	u8 nextdata = 0x00;
	KFS_Read(&nextdata, RESBaseaddr, 1);
	while (1)
	{
		if (nextdata != 0xff)
		{//数据不为空，开始处理
			if(KFS_read_data(tempaddr, &tempaddr) == 255)
			{
				//出现数据损坏
				return 255;
			}
			//读取结束后，tempaddr指向末尾位地址
			KFS_Read(&nextdata, tempaddr, 1);
			//读出末尾位判断是否正确
			if (nextdata == BOXSTOPbyte)
			{
				//地址加一
				tempaddr++;
				//读出当前位（应为开始位）
				KFS_Read(&nextdata, tempaddr, 1);
				if (nextdata == BOXSTARTbyte)
				{
					continue;//下一个周期
				}
				else
				{	//需要添加数据完整性判断
					return 255;//错误，下一包不是正确包（此时可能为处理结束，也可能为数据不完整）
					
				}
			}
			else
			{
				return 255;//错误，这一包结尾不正确（数据损坏）
			}
		}
		else
		{
			return 255;//错误，文件系统0地址为空（数据损坏）
		}	
	}
	
}


u8 KFS_read_data(u32 ReadAddr, u32* lastaddr)
{
	//起始位			id标志	id		数据地址标志		数据地 址		|	数据段		|	......		结束位(下一 个 databox开始)
	//BOXSTARTbyte	RESID	(0x00)	RESADDR			4x(0x00)		|	Nx(0x00)	|	......		BOXSTOPbyte	BOXSTARTbyte
	
	KFS_SetCur(ReadAddr);
	u8 temp = KFS_ReadByte();
	if (temp != BOXSTARTbyte)
	{
		//地址不是开始地址（数据损坏）
		return 0;	
	}
	else //找到了起始位
	{
		temp = KFS_ReadByte();
		if (temp == RESID)
		{
			u8 id = KFS_ReadByte();
			temp = KFS_ReadByte();
			if (temp == RESADDR)
			{
				u32 addr = 0;
				addr += KFS_ReadByte() << 24;
				addr += KFS_ReadByte() << 16;
				addr += KFS_ReadByte() << 8;
				addr += KFS_ReadByte();
				if (addr < 0x01 || addr > KFS_MAX_ADDR)
				{
					//数据地址超限（数据损坏）
					return 255;
				}
				//至此已经读到一个文件必要信息，写入目标文件的缓存里
//				printf("write para id = %2d, romaddr = 0x%8x\r\n",
//					id,
//					addr);
//				fflush(stdout);
				ICPX_write_file_addr_cache(id, addr);
				//将当前数据地址指针送向解析数据段函数
				*lastaddr = KFS_read_case(id, KFS_ReadCur());
				return id;
			}
			else
			{
				//数据地址标志格式错误（数据损坏）
				return 255;
			}
		}
		else
		{
			//数据id格式错误（数据损坏）
			return 255;
		}
	}

}	

u32 KFS_read_case(u8 id, u32 addr)
{
	u8 i,j,k,temp;
	u32 maxaddr = 0;
	//轮询各个参数，找到目标参数
	u8 FILEPARA_num;
	u8* FILEPARAS;
	if (id >= 100)
	{
		FILEPARA_num = sizeof(FILEPARASFONT);
		FILEPARAS = FILEPARASFONT;
	}
	else if (id < 100&& id != 0)
	{
		FILEPARA_num = sizeof(FILEPARASPIC);
		FILEPARAS = FILEPARASPIC;
	}
	else
	{
		return 0XDEEDBEEF;
	}
	for (i = 0; i < FILEPARA_num; i++)
	{
		KFS_SetCur(addr);
		for (j = 0; j < FILEPARA_num; j++)
		{
			temp = KFS_ReadByte();
			if (FILEPARAS[i] == temp)
			{
				//找到了目标值
				//读出数据长度
				u8 length = KFS_ReadByte();
				u8 datas[length+1];
				for (k = 0; k < length; k++)
				{
			 		datas[k] = KFS_ReadByte();
				}
//				if (length == 1)
//				{
//					printf("	para addr = %2x , length = %2d ,datas=0x%02X\r\n",
//						FILEPARAS[i],
//						length,
//						datas[0]);
//					fflush(stdout);
//				}
//				else
//				{
//					printf("	para addr = %2x , length = %2d ,datas=0x%02X%02X%02X%02X\r\n",
//						FILEPARAS[i],
//						length,
//						datas[0],
//						datas[1],
//						datas[2],
//						datas[3]);
//					fflush(stdout);
//				}
				//写入参数
				ICPX_write_file_para_cache(id, FILEPARAS[i], length, datas);
				
				//读到一个参数就已经完成了，不需要再读了
				break;
			}
			else if (FILEPARAS[i] == BOXSTOPbyte)
			{
				//数据末尾了，重新开始
				break;
			}
			else
			{
				//数据既不是结尾也不是当前需要的开头，跳过后续数据
				temp = KFS_ReadByte();//读到的是数据长度
				KFS_SetCur(KFS_ReadCur()+temp);
			}
			
		}
		if (maxaddr < KFS_ReadCur())
		{
			maxaddr = KFS_ReadCur();
		}
		
	}
	return maxaddr;
}

void KFS_Read(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
	W25QXX_FastRead(pBuffer,ReadAddr,NumByteToRead);
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
u8 KFS_ReadByte()													//读取一个byte并对地址自增
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