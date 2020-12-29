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
	ICPX_GPIO_Init();			//GPIO初始化
	Adc_Init();					//初始化adc
	NVIC_Configuration();		//中断向量初始化
	delay_init();				//按照系统时钟配置软延时
	BspTim2Init();				//按照系统时钟配置计时器（硬件计时）
	KEY_Init();					//按键io初始化
	CLI_INIT(9600);				//启动commandline

//	if(FLASH_GetReadOutProtectionStatus() != SET)
//	{
//		FLASH_ReadOutProtection(ENABLE);
//	}

	ICPX_Init_Spi_Bus();		//lcd和25 FLASH初始化
	turnonh3();
	turnonpm3();
	while (1) ;
	STARTMODETASK();			//开机模式判断
	
	ST7789_Fill(0, 0, ST7789_H, ST7789_W, BLACK);
	
	KFS_repair_fs();			//维护文件系统，重建内部文件信息缓存

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
		turnoffh3();
		ICPX_Charge_Screen(0);
		//充电动画循环实现
		CHGKEYTASK(1);
		MAINBATCHECKTASK(0);
		if (VCCvol < VCCTHRLOW)
		{
			//电源拔掉了
			ICPX_Standby();
		}
	}
	if (startmode == START_MODE_BAT)
	{
		static u8 boottimerneedreset = 0;
		if (!boottimerneedreset)
		{
			g_Tim2Array[eTim4] = 0;
			boottimerneedreset = 1;
		}
		turnonh3();
		if (isstarting == 1)		//1代表开机过程
		{
			ICPX_Booting_Screen(0);
			//ICPX_Shutdown_Screen(0);
			//启动动画实现
			//等待启动指令修改状态
			ICPX_Booting_Error_Screen(1);//初始化一次启动错误页面，让他可以在需要的时候刷背景
		}
		else if (isstarting == 2)	//2代表开机失败
		{
			ICPX_Booting_Error_Screen(0);
			CLI_RUN();
		}
		if (!IS_TIMEOUT_1MS(eTim4, 40000) && isstarting != 0)
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



