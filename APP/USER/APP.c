#include "ICOPYX_BSP.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "sys_command_line.h"

void dummy(){}
int main(void)
{
	Hsi_Init();	
	ICPX_GPIO_Init();			//GPIO初始化
	Adc_Init();					//初始化adc
	NVIC_Configuration();
	delay_init();
	BspTim2Init();
	KEY_Init();
	CLI_INIT(9600);			//启动commandline
	
//	if(FLASH_GetReadOutProtectionStatus() != SET)
//	{
//		FLASH_ReadOutProtection(ENABLE);  
//	}
	
	ICPX_Init_Spi_Bus();		//lcd和25 FLASH
	STARTMODETASK();			//开机模式判断
	ST7789_Fill(0, 0, ST7789_H, ST7789_W, BLACK);
	
//	while (1)
//	{	
//		ST7789_SetBaklight(100);
//		ST7789_DrawRectangle(0, 0, ST7789_H-1, ST7789_W-1, YELLOW);
//		g_Tim2Array[eTim1] = 0;
//		while (IS_TIMEOUT_1MS(eTim1, 500)) ;
//		ST7789_DrawRectangle(0, 0, ST7789_H-1, ST7789_W-1, BLACK);
//		g_Tim2Array[eTim1] = 0;
//		while (IS_TIMEOUT_1MS(eTim1, 500)) ;
//	}
	
	//ICPX_Test_25Q80();	//测试25flash
	KFS_repair_fs();	//从文件系统里读出文件
	
	//printf("%04X\r\n",GetMCUID()); //uuid
	//MCO_GPIO_Config();
	//MCO_OUT_Config();	
	
	while(1)
	{
		if (startmode == START_MODE_VCC)
		{
			turnoffh3();
			ICPX_Charge_Screen(0);
			//充电动画循环实现
			CHGKEYTASK(1);
			MAINBATCHECKTASK(0);
			if (VCCvol < VCCTHR)
			{//电源拔掉了
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
			else if(isstarting == 2)	//2代表开机失败
			{
				ICPX_Booting_Error_Screen(0);
				CLI_RUN();
			}
			if (!IS_TIMEOUT_1MS(eTim4, 40000) && isstarting != 0)
			{
				isstarting = 2;
			}
			if(isstarting == 0)			//0代表开机完成
			{
				boottimerneedreset = 0;
				MAINKEYTASK();
				MAINCHARGETASK(0);
				MAINBATCHECKTASK(0);
				ICPX_BAT_VOL_REVICE(0);
			}
			CLI_RUN();
		//主流程
		}
	}
}



