#include "AW87319.h"
#include "delay.h"

//初始化IIC接口
void AW87319_Init(void)
{
	IIC_Init();
	//检测芯片是否存在
	u8 temp = AW87319_Check();
	if (temp != 255)
	{
		//初始化AW87319
		AW87319_WriteOneByte(0x00, 0xAA);			//写入AA代表重置所有寄存器
		
		AW87319_WriteOneByte(0x01, 0B00000111);		//1（开启芯片软件）1（开启D类功放）1（开启boost升压）

													//BOOST升压电压
		AW87319_WriteOneByte(0x03, 0B00000100);		//111-----8.5V
													//110-----8.25V
													//101-----8.0V
													//100-----7.75V
		
													//BOOST电感峰值电流
		AW87319_WriteOneByte(0x04, 0B00000000);		//000-----1.5A
													//001-----2.00A
													//010-----2.50A
													//011-----3.00A
													//100-----3.25A
													//101-----3.50A
													//110-----3.75A
													//111-----4.00A
		
													//D类放大倍率
		AW87319_WriteOneByte(0x05, 0B00001001);		//		0000 : -3.0dB	Rini = 134kΩ
													//		0001 : -1.5dB	Rini = 113kΩ
													//		0010 : 0.0dB	Rini = 95kΩ
													//		0011 : 1.5dB	Rini = 80kΩ
													//		0100 : 3.0dB	Rini = 67kΩ
													//		0101 : 4.5dB	Rini = 57kΩ
													//		0110 : 6.0dB	Rini = 47kΩ
													//		0111 : 7.5dB	Rini = 40kΩ
													//		1000 : 9.0dB	Rini = 34kΩ
													//------这条线以上为boost关闭时可用的放大倍率
													//		1001 : 18.0dB	Rini = 18kΩ
													//		1010 : 19.5dB	Rini = 15kΩ
													//		1011 : 21.0dB	Rini = 13kΩ
													//		1100 : 22.5dB	Rini = 11kΩ
													//		1101 : 24.0dB	Rini = 9kΩ
													//		1110 : 25.5dB	Rini = 8kΩ
													//		1111 : 27.0dB	Rini = 6.5kΩ
													//------这条线以上为boost打开时可用的放大倍率
		
													//AGC3功率限制
		AW87319_WriteOneByte(0x06, 0B00000101);		//0000 : 0.5W @ 8Ω 0.67W @ 6Ω
													//0001 : 0.6W @ 8Ω 0.80W @ 6Ω
													//0010 : 0.7W @ 8Ω 0.93W @ 6Ω
													//0011 : 0.8W @ 8Ω 1.07W @ 6Ω
													//0100 : 0.9W @ 8Ω 1.20W @ 6Ω
													//0101 : 1.0W @ 8Ω 1.33W @ 6Ω
													//0110 : 1.1W @ 8Ω 1.47W @ 6Ω
													//0111 : 1.2W @ 8Ω 1.60W @ 6Ω
													//1000 : 1.3W @ 8Ω 1.73W @ 6Ω
													//1001 : 1.4W @ 8Ω 1.87W @ 6Ω
													//1010 : 1.5W @ 8Ω 2.00W @ 6Ω
													//1011 : AGC3 Disable
		
		AW87319_WriteOneByte(0x08, 0B00000000);
		AW87319_WriteOneByte(0x09, 0B00000010);
	}
}
//在AW87319指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 AW87319_ReadOneByte(u8 ReadAddr)
{				  
	u8 temp = 0;		  	    																 
	IIC_Start();
	IIC_Send_Byte(0XB0);		//发送器件地址0XB0,发送写数据 	 
	IIC_Wait_Ack();
	IIC_Send_Byte(ReadAddr);	//发送地址
	IIC_Wait_Ack();
	IIC_Stop();					//产生一个停止条件	  
	IIC_Start();
	IIC_Send_Byte(0XB1);        //发送器件地址0XB1,发送读数据 	 	   
	IIC_Wait_Ack();	 
	
	temp = IIC_Read_Byte(0);		
	IIC_Ack();
	IIC_Stop();//产生一个停止条件	    
	return temp;
}
//在AW87319指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void AW87319_WriteOneByte(u8 WriteAddr, u8 DataToWrite)
{				   	  	    																 
	IIC_Start();  
	IIC_Send_Byte(0XB0);			//发送器件地址0XB0,发送写数据 	 
	IIC_Wait_Ack();	   
	
	IIC_Send_Byte(WriteAddr);		//发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
	IIC_Stop();						//产生一个停止条件 
	delay_ms(10);	 
}
//在AW87319里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void AW87319_WriteLenByte(u8 WriteAddr, u32 DataToWrite, u8 Len)
{  	
	u8 t;
	for (t = 0;t < Len;t++)
	{
		AW87319_WriteOneByte(WriteAddr + t, (DataToWrite >> (8*t)) & 0xff);
	}												    
}

//在AW87319里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
u32 AW87319_ReadLenByte(u8 ReadAddr, u8 Len)
{  	
	u8 t;
	u32 temp = 0;
	for (t = 0;t < Len;t++)
	{
		temp <<= 8;
		temp += AW87319_ReadOneByte(ReadAddr + Len - t - 1); 	 				   
	}
	return temp;												    
}
//检查AW87319是否正常
//返回器件id
u8 AW87319_Check(void)
{   
	return AW87319_ReadOneByte(0);										  
}

//在AW87319里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AW87319_Read(u8 ReadAddr, u8 *pBuffer, u16 NumToRead)
{
	while (NumToRead)
	{
		*pBuffer ++= AW87319_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//在AW87319里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void AW87319_Write(u8 WriteAddr, u8 *pBuffer, u16 NumToWrite)
{
	while (NumToWrite--)
	{
		AW87319_WriteOneByte(WriteAddr, *pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}

