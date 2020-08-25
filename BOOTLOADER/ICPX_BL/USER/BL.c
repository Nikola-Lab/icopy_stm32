#include "stm32f10x.h" 
#include "BspTime.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "common.h"
#include "spi.h"
#include "ICOPYX_BSP.h"

extern pFunction Jump_To_Application;
extern uint32_t JumpAddress;
void GPIOINIT()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能B端口时钟
	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
	
	GPIO_InitStructure.GPIO_Pin = H3_PWR_ON_OFF_Pin;
	GPIO_Init(H3_PWR_ON_OFF_GPIO_Port, &GPIO_InitStructure);
	GPIO_SetBits(H3_PWR_ON_OFF_GPIO_Port, H3_PWR_ON_OFF_Pin);
	
}
void UART_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	//串口配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	// 配置 USART1 Tx (PA.09) 作为功能引脚并上拉输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//配置 USART1 Tx (PA.10) 作为功能引脚并是浮空输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);
}
int main(void) 	
{   
	SCB->VTOR = FLASH_BASE | 0x10000;//中断向量表偏移
	//由于本机是app，中断要按照原有位置偏移	
	Hsi_Init();
	BspTim2Init();
	GPIOINIT();
	UART_Init();
	SPI1_Init();
	W25QXX_Init();
	//ICPX_Init_Spi_Bus();	//lcd和25 FLASH
	//SerialPutString("Start_bootloader\r\n");
	//ICPX_Find_25Q80();
	Main_Menu();
	
	
	 
	if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000) == 0x20000000)
	{
		SerialPutString("Execute user Program \r\n*********************************************************\r\n");
		BspTim2Close();
		//跳转至用户代码
		JumpAddress = *(__IO uint32_t*)(ApplicationAddress + 4);
		Jump_To_Application = (pFunction) JumpAddress;
		//初始化用户程序的堆栈指针
		__set_MSP(*(__IO uint32_t*) ApplicationAddress);
		Jump_To_Application();
	}
	else
	{
		SerialPutString("no user Program\r\n");
	}
	
	while (1) ; 
}
