#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "ICOPYX_BSP.h"
#include "BspTime.h"




//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
	//pa15 pb3 pb4 释放为普通io

	//上拉模式
	GPIO_InitStructure.GPIO_Pin  = BUTRIGHT_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(BUTRIGHT_GPIO_Port, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = BUTUP_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(BUTUP_GPIO_Port, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = BUTOK_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(BUTOK_GPIO_Port, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = BUTDOWN_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(BUTDOWN_GPIO_Port, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = BUTLEFT_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(BUTLEFT_GPIO_Port, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = BUTM1_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(BUTM1_GPIO_Port, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = BUTM2_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(BUTM2_GPIO_Port, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = BUTALL_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(BUTALL_GPIO_Port, &GPIO_InitStructure);
	
	//下拉模式
	GPIO_InitStructure.GPIO_Pin  = WKUP_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(WKUP_GPIO_Port, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = BUTPWR_CAN_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(BUTPWR_CAN_GPIO_Port, &GPIO_InitStructure);

}
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//注意响应优先级
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up = 1;//按键按松开标志
	if (mode)key_up = 1;  //支持连按
	if(key_up&&(\
		KEYUP == 0\
		|| KEYDOWN == 0\
		|| KEYRIGHT == 0\
		|| KEYLEFT == 0\
		|| KEYOK == 0\
		|| KEYM1 == 0\
		|| KEYM2 == 0\
		|| KEY_ALL == 0\
		|| KEY_PWR_CAN == 1))
	{//抬起状态时有任意按钮按下
		delay_ms(5);//去抖动 
		key_up = 0;
		if (KEYUP == 0)return KEYUP_PRES;
		else if (KEYDOWN == 0)return KEYDOWN_PRES;
		else if (KEYRIGHT == 0)return KEYRIGHT_PRES;
		else if (KEYLEFT == 0)return KEYLEFT_PRES;
		else if (KEYOK == 0)return KEYOK_PRES;
		else if (KEYM1 == 0)return KEYM1_PRES;
		else if (KEYM2 == 0)return KEYM2_PRES;
		else if (KEY_PWR_CAN == 1)return KEY_PWR_CAN_PRES;
		else if (KEY_ALL == 0)return KEY_ALL_PRES;
	}else if(//按下状态时或者所有按钮都抬起
		KEYUP == 1\
		&& KEYDOWN == 1\
		&& KEYRIGHT == 1\
		&& KEYLEFT == 1\
		&& KEYOK == 1\
		&& KEYM1 == 1\
		&& KEYM2 == 1\
		&& KEY_ALL == 1\
		&& KEY_PWR_CAN == 0)
	{//所有按钮都抬起
		key_up = 1; 	  
	}
	//else按下状态时什么都不做
	return 0;// 无按键按下
}
u16 KEY_POWER(u8 clean)
{	 
	static u16 downtime = 0;
	if (clean) 
	{
		g_Tim2Array[eTim1] = 0;
		downtime = 0;
	}
	if(KEY_PWR_CAN == 1)
	{
		delay_ms(5);//去抖动 
		if (KEY_PWR_CAN == 1)
		{
			
			downtime = g_Tim2Array[eTim1];
			return downtime;
		}

	}
	else if(KEY_PWR_CAN == 0)
	{//抬起
		g_Tim2Array[eTim1] = 0;
		downtime = 0;
	}
	return 0;// 无按键按下
}
