#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"
void Adc_Init(void);
u16  Get_Adc(u8 ch); 
u16 Get_Adc_Average(u8 ch,u8 times); 
u16 Get_Adc1c(u8 ch);
u16 Start_Adc(u8 ch);
u16 Read_Adc_Status();
u16 Read_Adc_Data();
u16 Read_Adc_RUNNING();
void Adc_Reinit(void);
#endif 
