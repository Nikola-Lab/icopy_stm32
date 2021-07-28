#include "ICOPYX_BSP.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "sys_command_line_test.h"
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
	
	ICPX_Init_Spi_Bus();		//lcd和25 FLASH初始化
	
	ST7789_Fill(0, 0, ST7789_H, ST7789_W, GREEN);
	
	ICPX_CHG_CUR_SET_AIM(1);	//设定高速模式
	ICPX_CHG_CUR_SET(4);	//设定高速模式
	//测试主板用的代码
	//turnonh3();
	//turnonpm3();
	//while (1) ;
	
	//ST7789_BL_ON();
	//KFS_POWERON_SEARCH();		//维护文件系统，重建内部文件信息缓存
}
/******************************************************************************
      函数说明： loop大循环，系统正常运行时无限循环
      入口数据：	无
      返回值：   无
******************************************************************************/
void loop()
{
	static u8 flagred = 0;
	if (testkeying == 1)
	{
		MAINKEYTASK();
	}
	if (stdnflag == 1)
	{
		ICPX_Standby();
	}
	if (g_Tim2Array[eTim1] >= 300 && flagred == 0)
	{
		ST7789_Fill(0, 0, 240, 240, BLACK);
		flagred = 1;
	}
	if (g_Tim2Array[eTim1] >= 600 && flagred == 1)
	{
		g_Tim2Array[eTim1] = 0;
		ST7789_Fill(0, 0, 240, 240, GREEN);
		flagred = 0;
	}
	CLI_RUN();
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



