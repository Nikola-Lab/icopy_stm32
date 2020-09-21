#include "ICOPYX_BSP.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "sys_command_line.h"


// TODO: 添加方向键长按处理
// 添加加解密算法
// 添加屏幕权限切换
// 添加bl模式下的屏幕切换
// 添加资源库自动解析
// 充电状态回报
// 添加重新初始化屏幕的指令


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
	
	ICPX_Test_25Q80();	//测试25flash
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
			CHGKEYTASK();
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
			}
			else if(isstarting == 2)	//2代表开机失败
			{
				ICPX_Booting_Error_Screen();
			}
			//if (!IS_TIMEOUT_1MS(eTim4, 10000))
			//{
			//	isstarting = 2;
			//}
			CLI_RUN();
			MAINKEYTASK();
			MAINCHARGETASK(0);
			//MAINBATCHECKTASK();
		//主流程
		}
	}
}



