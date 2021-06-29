#include "IP5306.h"
#include "delay.h"

//初始化IIC接口
void IP5306_Init(void)
{
	IIC_Init();
}
//在IP5306指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 IP5306_ReadOneByte(u8 ReadAddr)
{				  
	u8 temp = 0;		  	    																 
	IIC_Start();
	IIC_Send_Byte(0XEA);		//发送器件地址0XB0,发送写数据 	 
	IIC_Wait_Ack();
	IIC_Send_Byte(ReadAddr);	//发送地址
	IIC_Wait_Ack();
	IIC_Stop();					//产生一个停止条件	  
	IIC_Start();
	IIC_Send_Byte(0XEB);        //发送器件地址0XB1,发送读数据 	 	   
	IIC_Wait_Ack();	 
	
	temp = IIC_Read_Byte(0);		
	IIC_Ack();
	IIC_Stop();//产生一个停止条件	    
	return temp;
}
//在IP5306指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void IP5306_WriteOneByte(u8 WriteAddr, u8 DataToWrite)
{				   	  	    																 
	IIC_Start();  
	IIC_Send_Byte(0XEA);			//发送器件地址0XB0,发送写数据 	 
	IIC_Wait_Ack();	   
	
	IIC_Send_Byte(WriteAddr);		//发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
	IIC_Stop();						//产生一个停止条件 
	delay_ms(10);	 
}
//在IP5306里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void IP5306_WriteLenByte(u8 WriteAddr, u32 DataToWrite, u8 Len)
{  	
	u8 t;
	for (t = 0;t < Len;t++)
	{
		IP5306_WriteOneByte(WriteAddr + t, (DataToWrite >> (8*t)) & 0xff);
	}												    
}

//在IP5306里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
u32 IP5306_ReadLenByte(u8 ReadAddr, u8 Len)
{  	
	u8 t;
	u32 temp = 0;
	for (t = 0;t < Len;t++)
	{
		temp <<= 8;
		temp += IP5306_ReadOneByte(ReadAddr + Len - t - 1); 	 				   
	}
	return temp;												    
}
//检查IP5306是否正常
//返回器件id
u8 IP5306_Check(void)
{   
	return IP5306_ReadOneByte(0);										  
}

//在IP5306里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void IP5306_Read(u8 ReadAddr, u8 *pBuffer, u16 NumToRead)
{
	while (NumToRead)
	{
		*pBuffer ++= IP5306_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//在IP5306里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void IP5306_Write(u8 WriteAddr, u8 *pBuffer, u16 NumToWrite)
{
	while (NumToWrite--)
	{
		IP5306_WriteOneByte(WriteAddr, *pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}

