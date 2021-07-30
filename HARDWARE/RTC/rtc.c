#include "sys.h"
#include "delay.h"
#include "rtc.h"
#include <stdio.h>
#include "ICOPYX_BSP.h"
//实时时钟配置
//初始化RTC时钟,同时检测时钟是否工作正常
//BKP->DR2用于保存是否第一次配置的设置
u8 RTC_Init(void)
{
	//检查是不是第一次配置时钟
	u8 temp = 0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟   
	PWR_BackupAccessCmd(ENABLE);	//使能后备寄存器访问  
	if(
		(BKP_ReadBackupRegister(BKP_DR2) != 0x5050) ||
		(
			(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) && 
			(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
		)
	  ) //bkpdr2 数据不正确,或者是两个时钟都失效了，意味着是第一次上电或者时钟失效后的上电
	{	 			
		RCC_LSEConfig(RCC_LSE_ON);
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET&&temp < 250)	//检查指定的RCC标志位设置与否,等待低速晶振就绪
			{
				temp++;
				delay_ms(10);
			}
		if (temp >= 250)//初始化时钟失败,晶振有问题		
		{
			printf("rtc: use hsi\r\n");
			rtc_ext = 0;
			fflush(stdout);
			RCC_LSEConfig(RCC_LSE_OFF);
			RCC_LSICmd(ENABLE);
			temp = 0;
			while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET&&temp < 250)	//检查指定的RCC标志位设置与否,等待低速晶振就绪
				{
					temp++;
					delay_ms(10);
				}
			if (temp >= 250) return 1;//初始化时钟失败,晶振有问题		
			RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
		}
		else//初始化时钟成功	
		{
			rtc_ext = 1;
			printf("rtc: use hse\r\n");
			fflush(stdout);
			RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		}
		
		RCC_RTCCLKCmd(ENABLE);	//启动RTC时钟
		RTC_WaitForLastTask();
		RTC_WaitForSynchro();
		//RTC_ITConfig(RTC_IT_SEC, ENABLE);		//使能RTC秒中断
		//RTC_WaitForLastTask();
		
		RTC_EnterConfigMode();
		RTC_SetPrescaler(40000); //设置预分频
		RTC_WaitForLastTask();
		RTC_Set(0);	
		printf("rtc: set zero\r\n");
		fflush(stdout);
		RTC_ExitConfigMode();
		BKP_WriteBackupRegister(BKP_DR2, 0X5050);//写个标志，防止下次开机改
	}
	else//到这里说明系统已经配置过了
	{
		printf("rtc: already setup,time:%lu,LSE:%d,LSI:%d\r\n",RTC_Get(),
			RCC_GetFlagStatus(RCC_FLAG_LSERDY),
			RCC_GetFlagStatus(RCC_FLAG_LSIRDY)
			);
		fflush(stdout);
	}				     
	return 0;
}

//设置时钟
u8 RTC_Set(u32 sec)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	PWR_BackupAccessCmd(ENABLE);	//使能RTC和后备寄存器访问 
	RTC_SetCounter(sec);			//设置计数器
	RTC_WaitForLastTask();			//等待RTC寄存器操作完成  	
	return 0;	    
}

//得到当前的时间

u32 RTC_Get(void)
{  
	return RTC_GetCounter();	  
} 
















