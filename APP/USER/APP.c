#include "ICOPYX_BSP.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "sys_command_line.h"
/******************************************************************************
      函数说明： dummy填充函数，无任何作用
      入口数据：	无
      返回值：   无
******************************************************************************/
void dummy() {}
/******************************************************************************
      函数说明： setup函数，系统每次开机执行一次
      入口数据：	无
      返回值：   无
******************************************************************************/
void setup()
{
#define setup_flag

	
	Hsi_Init();					//内部时钟初始化
	delay_init();				//按照系统时钟配置软延时
	ICPX_GPIO_Init();			//GPIO初始化
	Adc_Init();					//初始化adc
	NVIC_Configuration();		//中断向量初始化
	BspTim2Init();				//按照系统时钟配置计时器（硬件计时）
	KEY_Init();					//按键io初始化
	CLI_INIT(57600);			//启动commandline
	ICPX_BKP_Init();			//初始化备份寄存器
	RTC_Init();					//初始化rtc
	ICPX_Init_Spi_Bus();		//lcd和25 FLASH初始化
	
	STARTMODETASK();			//开机模式判断
	
	ST7789_Fill(0, 0, ST7789_H, ST7789_W, BLACK);	//启动之前先给屏幕刷黑，防止闪烁
	
	KFS_POWERON_SEARCH();		//维护文件系统，重建内部文件信息缓存

	
//		//测试电压采集用的代码
//		ST7789_BL_ON();
//		u16 temp, temp2, temp3, temp4;
//		while (1) 
//		{
//			temp = Intvolavl;
//			temp2 = BATvolavl;
//			temp3 = (u16)(4915200 / temp);
//			temp4 = VCCvolavl;
//			ST7789_ShowIntNum(60, 100, temp4, 10, WHITE, BLACK, 24);	//usb电压
//			ST7789_ShowIntNum(60, 130, temp, 10, WHITE, BLACK, 24);		//内部基准电压（读出值
//			ST7789_ShowIntNum(60, 160, temp3, 10, WHITE, BLACK, 24);	//供电vcc电压
//			ST7789_ShowIntNum(60, 200, temp2, 10, WHITE, BLACK, 24);	//电池输入电压
//			
//		}

	
	//printf("%04X\r\n",GetMCUID()); //uuid
	//MCO_GPIO_Config();
	//MCO_OUT_Config();

}
/******************************************************************************
      函数说明： loop大循环，系统正常运行时无限循环
      入口数据：	无
      返回值：   无
******************************************************************************/
void loop()
{
	if (startmode == START_MODE_VCC)                                         
	{
		//vcc唤醒意味着进入充电模式了
		turnoffh3();
		if (ICPX_img_data_ok == 1)
		{
			ICPX_Charge_Screen(0);//充电动画循环实现
		}
		else
		{
			ST7789_BL_ON();
			ST7789_Fill(0, 0, ST7789_H, ST7789_W, GREEN);	//没有资源的情况下，充电显示绿屏
		}
		ICPX_CHG_CUR_SET_AIM(1);//设定高速模式
		MAINCHARGETASK(0);
		CHGKEYTASK(1);
		MAINBATCHECKTASK(0);
		//检测电压来判断是否关机
		if (VCCvol < VCCTHRLOW)
		{
			//电源拔掉了
			ICPX_Standby();
		}
	}
	if (startmode == START_MODE_BAT)
	{	//bat唤醒是正常开机
		static u8 boottimerneedreset = 0;
		if (!boottimerneedreset)
		{
			g_Tim2Array[eTim4] = 0;
			boottimerneedreset = 1;
		}
		turnonh3();
		if (isstarting == 1)		//1代表开机过程
		{
			ICPX_CHG_CUR_SET_AIM(0);//设定低速模式
			if (ICPX_img_data_ok == 1)
			{
				ICPX_Booting_Screen(0);
				//ICPX_Shutdown_Screen(0);
				//启动动画实现
				//等待启动指令修改状态
				ICPX_Booting_Error_Screen(1);//初始化一次启动错误页面，让他可以在需要的时候刷背景
			}
			else
			{
				ST7789_BL_ON();
				ST7789_Fill(0, 0, ST7789_H, ST7789_W, WHITE);	//没有资源的情况下，开机显示白屏
			}
		}
		else if (isstarting == 2)	//2代表开机失败
		{
			if (ICPX_img_data_ok == 1)
			{
				ICPX_Booting_Error_Screen(0);
			}
			else
			{
				ST7789_BL_ON();
				ST7789_Fill(0, 0, ST7789_H, ST7789_W, RED);	//没有资源的情况下，开机失败显示红屏
			}
			MAINBATCHECKTASK(0);
			CLI_RUN();
		}
		if (!IS_TIMEOUT_1MS(eTim4, 65000) && isstarting != 0)
		{
			isstarting = 2;
		}
		if (isstarting == 0)			//0代表开机完成
		{
			boottimerneedreset = 0;
			MAINKEYTASK();
			MAINCHARGETASK(0);
			MAINBATCHECKTASK(0);
			ICPX_BAT_VOL_REVICE(0);
		}
		CHARGE_OTG();
		CLI_RUN();
		
		//主流程
	}
}

/******************************************************************************
      函数说明： app系统主函数，调用setup和loop工作
      入口数据：	无
      返回值：   无
******************************************************************************/
int main(void)
{
#ifdef setup_flag
	setup();
#endif
	while (1)
	{
		loop();
	}
}



