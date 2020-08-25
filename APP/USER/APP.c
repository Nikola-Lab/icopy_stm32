#include "ICOPYX_BSP.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "sys_command_line.h"
void dummy(){}
int main(void)
{
	u16 led0pwmval = 10;
	u8 dir = 1;
	
	
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
	//W25QXX_Erase_Chip();
	//ICPX_BB_25Q80(0);
	//uart_init(115200);		//串口1初始化
	//	printf("Core start!\r\n");
	//	fflush(stdout);			//gnugcc特性
	ICPX_Test_25Q80();	//测试25flash
	//printf("%04X\r\n",GetMCUID()); //uuid
	//MCO_GPIO_Config();
	//MCO_OUT_Config();	
	
	
	while(1)
	{
		if (startmode == START_MODE_VCC)
		{
			GPIO_ResetBits(H3_PWR_ON_OFF_GPIO_Port, H3_PWR_ON_OFF_Pin);
			ICPX_Charge_Screen(0);
			CHGKEYTASK();
			if (VCCvol < VCCTHR)
			{//电源拔掉了
				ICPX_Standby();
			}
			//充电动画循环实现
			//充电循环流程
		}
		if (startmode == START_MODE_BAT)
		{
			static u8 boottimerneedreset = 0;
			if (!boottimerneedreset)
			{
				g_Tim2Array[eTim4] = 0;
				boottimerneedreset = 1;
			}
			GPIO_SetBits(H3_PWR_ON_OFF_GPIO_Port, H3_PWR_ON_OFF_Pin);
			if (isstarting == 1)
			{	
				ICPX_Booting_Screen(0);
				//启动动画实现
				//等待启动指令修改状态
			}
			else if(isstarting == 2)
			{
				ICPX_Booting_Error_Screen();
			}
			//if (!IS_TIMEOUT_1MS(eTim4, 10000))
			//{
			//	isstarting = 2;
			//}
			CLI_RUN();
			MAINKEYTASK();
			MAINCHARGETASK();
			MAINBATCHECKTASK();
		//主流程
		}
	}
}



