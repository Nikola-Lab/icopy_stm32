#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

#define KEYUP			GPIO_ReadInputDataBit(BUTUP_GPIO_Port,BUTUP_Pin)
#define KEYDOWN			GPIO_ReadInputDataBit(BUTDOWN_GPIO_Port,BUTDOWN_Pin)
#define KEYRIGHT		GPIO_ReadInputDataBit(BUTRIGHT_GPIO_Port,BUTRIGHT_Pin)
#define KEYLEFT			GPIO_ReadInputDataBit(BUTLEFT_GPIO_Port,BUTLEFT_Pin)
#define KEYOK			GPIO_ReadInputDataBit(BUTOK_GPIO_Port,BUTOK_Pin)
#define KEYM1			GPIO_ReadInputDataBit(BUTM1_GPIO_Port,BUTM1_Pin)
#define KEYM2			GPIO_ReadInputDataBit(BUTM2_GPIO_Port,BUTM2_Pin)
#define KEY_PWR_CAN		GPIO_ReadInputDataBit(BUTPWR_CAN_GPIO_Port,BUTPWR_CAN_Pin)
#define KEY_ALL			GPIO_ReadInputDataBit(BUTALL_GPIO_Port,BUTALL_Pin)

#define KEY_WKUP		GPIO_ReadInputDataBit(WKUP_GPIO_Port,WKUP_Pin)

 
	
#define KEYUP_PRES 			1	
#define KEYDOWN_PRES		2	
#define KEYRIGHT_PRES		3	
#define KEYLEFT_PRES		4
#define KEYOK_PRES			5
#define KEYM1_PRES			6
#define KEYM2_PRES			7
#define KEY_PWR_CAN_PRES	8
#define KEY_ALL_PRES		9
#define KEY_WKUP_PRES 		10

void KEY_Init(void);	//IO初始化
u8 KEY_Scan(u8 mode);  	//按键扫描函数
u16 KEY_POWER(u8 clean);
u16 KEY_ICPY_UP(u8 clean);
u16 KEY_ICPY_DOWN(u8 clean);
u16 KEY_ICPY_LEFT(u8 clean);
u16 KEY_ICPY_RIGHT(u8 clean);
	
#endif
