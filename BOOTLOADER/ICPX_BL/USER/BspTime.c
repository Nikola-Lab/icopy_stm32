
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_flash.h"
#include "BspTime.h"

volatile int g_Tim2Array[(u16)eTimMax] = {0};

//定时器初始化
void BspTim2Init(void)
{
    NVIC_InitTypeDef NvicInitdef;
    TIM_TimeBaseInitTypeDef timbase;	
    uc16 cnt = 1000;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	
	//APB1 32M,使用了2分频器，所以自动倍频到64m
    TIM_DeInit(TIM2); 
    timbase.TIM_CounterMode = TIM_CounterMode_Up;
    timbase.TIM_ClockDivision = TIM_CKD_DIV1;
    timbase.TIM_Period = cnt - 1;
	//计数数量1000
    timbase.TIM_Prescaler = 64 - 1;
	//分频系数64-1，也就是计数频率64M/64=1M
    TIM_TimeBaseInit(TIM2, &timbase);
	//1M/1000=1khz，1ms一次

    NvicInitdef.NVIC_IRQChannel = TIM2_IRQn;
    NvicInitdef.NVIC_IRQChannelPreemptionPriority = 10;
    NvicInitdef.NVIC_IRQChannelSubPriority = 0;
    NvicInitdef.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NvicInitdef);

    TIM_SetCounter(TIM2, 0);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}
//关闭定时器
void BspTim2Close(void)
{
    TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
    TIM_Cmd(TIM2, DISABLE);
}

void TIM2_IRQHandler(void)
{
    u16 i = 0;

    if( SET == TIM_GetITStatus(TIM2,TIM_IT_Update) )
    {
        TIM_ClearITPendingBit( TIM2, TIM_IT_Update);

        for (i = 0; i < (u16)eTimMax; i++)
        {
            g_Tim2Array[i]++;
        }
    }
}

