#include "sys.h"
#include "stm32f10x.h"
#include "stm32f10x_flash.h"

void NVIC_Configuration(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级

}
/*
* 初始化 MCO 引脚 PA8
*/
void MCO_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
void MCO_OUT_Config(void)
{	//设置 MCO 引脚输出时钟
    //可以是 HSE,HSI,PLLCLK/2,SYSCLK
    //RCC_MCOConfig(RCC_MCO_HSE);
    //RCC_MCOConfig(RCC_MCO_HSI);
    //RCC_MCOConfig(RCC_MCO_PLLCLK_Div2);
	RCC_MCOConfig(RCC_MCO_SYSCLK);
}
void Hsi_Init(void)
{
	//RCC_DeInit();//将外设 RCC寄存器重设为缺省值  
	RCC_HSICmd(ENABLE);//使能HSI    
	while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET) ;//等待HSI使能成功  
    
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);      //使能或者失能预取指缓存（参数里是使能）    -----加上这两句才能到64M
	FLASH_SetLatency(FLASH_Latency_2);		//设置代码延时值（参数里的是指2延时周期）
     
	RCC_HCLKConfig(RCC_SYSCLK_Div1);		//设置AHB时钟 HCLK = SYSCLK/1    -----这里频率是64/1 = 64M   
	RCC_PCLK1Config(RCC_HCLK_Div2);			//设置低速APB1时钟        -----这里频率是64/2 = 32M     定时器2~7频率是24M
	RCC_PCLK2Config(RCC_HCLK_Div1);			//设置高速APB2时钟        -----这里频率是64/1 = 64M
      
	//设置 PLL 时钟源及倍频系数  
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16);//使能或者失能 PLL,这个参数可以取：ENABLE或者DISABLE  
	//也就是64M
	RCC_PLLCmd(ENABLE);//如果PLL被用于系统时钟,那么它不能被失能  
	//等待指定的 RCC 标志位设置成功 等待PLL初始化成功  
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);  
 
	//设置系统时钟（SYSCLK） 设置PLL为系统时钟源  
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);//选择想要的系统时钟   
	//等待PLL成功用作于系统时钟的时钟源  
	//  0x00：HSI 作为系统时钟   
	//  0x04：HSE作为系统时钟   
	//  0x08：PLL作为系统时钟    
	while(RCC_GetSYSCLKSource() != 0x08);//需与被选择的系统时钟对应起来，RCC_SYSCLKSource_PLL
}



