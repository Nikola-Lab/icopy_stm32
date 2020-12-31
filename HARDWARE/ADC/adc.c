#include "adc.h"
#include "delay.h"		
#include "ICOPYX_BSP.h"

void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);	  //使能ADC1通道时钟
 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);									//设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PA1和2 模拟输入 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	                      
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;					//独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;						//单通道,不扫描
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;					//单次转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//软件触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;				//右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;								//规则通道数量
	ADC_Init(ADC1, &ADC_InitStructure); 

	//ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
  
	ADC_Cmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);											//复位校准  
	
	delay_ms(10);														//听说GD得加这个
	 
	while(ADC_GetResetCalibrationStatus(ADC1));							//等待结束
	
	ADC_StartCalibration(ADC1);											//AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));								//等待结束
	
	ADC_TempSensorVrefintCmd(ENABLE);									//启用内部温度传感器和基准电压源
}				  
//获得ADC值
//ch:通道值 0~3
u16 Get_Adc(u8 ch)   
{
	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);		    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);								//转换启动	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));						//等待转换结束
	
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);									//清除转换完成标记（GD需要）
	
	return ADC_GetConversionValue(ADC1);								//返回规则组结果
}

u16 Get_Adc_Average(u8 ch, u8 times)
{
	//if (ch == ADC_Channel_17)
	//Adc_Reinit();
	if (times == 0)return 0;
	u32 temp_val = 0;
	u8 t;
	for (t = 0;t < times;t++)
	{
		temp_val += Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val / times;
} 	 

u16 Get_Adc1c(u8 ch)
{
	return Get_Adc(ch);
}
void  Adc_Reinit(void)
{
	ADC_InitTypeDef ADC_InitStructure; 
	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;					//独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;						//单通道,不扫描
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;					//单次转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//软件触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;				//右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;								//规则通道数量
	ADC_Init(ADC1, &ADC_InitStructure); 
	ADC_Cmd(ADC1, ENABLE);
	ADC_TempSensorVrefintCmd(ENABLE);									//启用内部温度传感器和基准电压源
}
