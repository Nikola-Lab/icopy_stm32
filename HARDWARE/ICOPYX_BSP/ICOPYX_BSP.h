#ifndef __ICPX_BSP_H
#define __ICPX_BSP_H


#include "ICOPYX_IMAGE.H"
#include "sys.h"
#include "string.h"
#include "delay.h"
#include <stdio.h>
#include "w25qxx.h"	 
#include "ST7789_init.h"
#include "ST7789.h"
#include "stm32f10x.h"
#include "KombiFs.h"
#include "adc.h"
#include "key.h"
#include "BspTime.h"
#include "stm32f10x_pwr.h"
#include "math.h"
#include "timer.h"
#include "sys_command_line.h"

#define BLAddress 0x8010000
#ifdef v10
#define WKUP_Pin 							GPIO_Pin_0
#define WKUP_GPIO_Port 						GPIOA 
												  
#define VUSB_Pin  							GPIO_Pin_1
#define VUSB_GPIO_Port 						GPIOA 
#define VBAT_Pin 							GPIO_Pin_2
#define VBAT_GPIO_Port 						GPIOA 
												  
#define CHARG_STATE_Pin 					GPIO_Pin_15
#define CHARG_STATE_GPIO_Port 				GPIOB 
												  
												  
#define SWDIO_Pin 							GPIO_Pin_13
#define SWDIO_GPIO_Port 					GPIOA 
#define SWCLK_Pin 							GPIO_Pin_14
#define SWCLK_GPIO_Port 					GPIOA 
												  
												  
#define BUTRIGHT_Pin 						GPIO_Pin_12
#define BUTRIGHT_GPIO_Port 					GPIOA 
#define BUTUP_Pin 							GPIO_Pin_15
#define BUTUP_GPIO_Port 					GPIOA 
#define BUTOK_Pin 							GPIO_Pin_3
#define BUTOK_GPIO_Port 					GPIOB 
#define BUTDOWN_Pin 						GPIO_Pin_4
#define BUTDOWN_GPIO_Port 					GPIOB 
#define BUTLEFT_Pin 						GPIO_Pin_5
#define BUTLEFT_GPIO_Port 					GPIOB 
#define BUTM1_Pin 							GPIO_Pin_6
#define BUTM1_GPIO_Port			 			GPIOB 
#define BUTM2_Pin 							GPIO_Pin_11
#define BUTM2_GPIO_Port 					GPIOA 
#define BUTPWR_CAN_Pin 						GPIO_Pin_7
#define BUTPWR_CAN_GPIO_Port 				GPIOB 
#define BUTALL_Pin 							GPIO_Pin_10
#define BUTALL_GPIO_Port 					GPIOB 
												  
#define H3_PWR_ON_OFF_Pin 					GPIO_Pin_8
#define H3_PWR_ON_OFF_GPIO_Port 			GPIOB 
#define PM_PWR_ON_OFF_Pin 					GPIO_Pin_9
#define PM_PWR_ON_OFF_GPIO_Port 			GPIOB 
												  
#define PM_BUTTON_Pin 						GPIO_Pin15
#define PM_BUTTON_GPIO_Port 				GPIOC
												  
#define PM_4LED_Pin 						GPIO_Pin_14
#define PM_4LED_GPIO_Port 					GPIOB
												 
#define LCD_DC_Pin 							GPIO_Pin_3
#define LCD_DC_GPIO_Port 					GPIOA
#define LCD_CS_Pin 							GPIO_Pin_4
#define LCD_CS_GPIO_Port 					GPIOA
#define LCD_SCL_Pin 						GPIO_Pin_5
#define LCD_SCL_GPIO_Port 					GPIOA
#define LCD_RST_Pin 						GPIO_Pin_6
#define LCD_RST_GPIO_Port 					GPIOA
#define LCD_SDA_Pin 						GPIO_Pin_7
#define LCD_SDA_GPIO_Port 					GPIOA
#define LCD_PWR_Pin 						GPIO_Pin_0
#define LCD_PWR_GPIO_Port 					GPIOB
#define LCD_BKLT_Pin 						GPIO_Pin_8
#define LCD_BKLT_GPIO_Port 					GPIOA
#endif

#ifdef v11
#define WKUP_Pin 							GPIO_Pin_0
#define WKUP_GPIO_Port 						GPIOA 
			
#define FLASH_PWR_Pin 						GPIO_Pin_11
#define FLASH_PWR_GPIO_Port 				GPIOB 

#define VUSB_Pin  							GPIO_Pin_1
#define VUSB_GPIO_Port 						GPIOA 
#define VBAT_Pin 							GPIO_Pin_2
#define VBAT_GPIO_Port 						GPIOA 
												  
#define CHARG_STATE_Pin 					GPIO_Pin_15
#define CHARG_STATE_GPIO_Port 				GPIOB 
												  
												  
#define SWDIO_Pin 							GPIO_Pin_13
#define SWDIO_GPIO_Port 					GPIOA 
#define SWCLK_Pin 							GPIO_Pin_14
#define SWCLK_GPIO_Port 					GPIOA 
												  
												  
#define BUTRIGHT_Pin 						GPIO_Pin_12
#define BUTRIGHT_GPIO_Port 					GPIOA 
#define BUTUP_Pin 							GPIO_Pin_15
#define BUTUP_GPIO_Port 					GPIOA 
#define BUTOK_Pin 							GPIO_Pin_3
#define BUTOK_GPIO_Port 					GPIOB 
#define BUTDOWN_Pin 						GPIO_Pin_4
#define BUTDOWN_GPIO_Port 					GPIOB 
#define BUTLEFT_Pin 						GPIO_Pin_5
#define BUTLEFT_GPIO_Port 					GPIOB 
#define BUTM1_Pin 							GPIO_Pin_6
#define BUTM1_GPIO_Port			 			GPIOB 
#define BUTM2_Pin 							GPIO_Pin_11
#define BUTM2_GPIO_Port 					GPIOA 
#define BUTPWR_CAN_Pin 						GPIO_Pin_7
#define BUTPWR_CAN_GPIO_Port 				GPIOB 
#define BUTALL_Pin 							GPIO_Pin_10
#define BUTALL_GPIO_Port 					GPIOB 
												  
#define H3_PWR_ON_OFF_Pin 					GPIO_Pin_8
#define H3_PWR_ON_OFF_GPIO_Port 			GPIOB 
#define PM_PWR_ON_OFF_Pin 					GPIO_Pin_9
#define PM_PWR_ON_OFF_GPIO_Port 			GPIOB 
												  
#define PM_BUTTON_Pin 						GPIO_Pin_15
#define PM_BUTTON_GPIO_Port 				GPIOC
												  
#define PM_4LED_Pin 						GPIO_Pin_14
#define PM_4LED_GPIO_Port 					GPIOB
												 
#define LCD_DC_Pin 							GPIO_Pin_3
#define LCD_DC_GPIO_Port 					GPIOA
#define LCD_CS_Pin 							GPIO_Pin_4
#define LCD_CS_GPIO_Port 					GPIOA
#define LCD_SCL_Pin 						GPIO_Pin_5
#define LCD_SCL_GPIO_Port 					GPIOA
#define LCD_RST_Pin 						GPIO_Pin_0
#define LCD_RST_GPIO_Port 					GPIOB
#define LCD_SDA_Pin 						GPIO_Pin_7
#define LCD_SDA_GPIO_Port 					GPIOA
#define LCD_PWR_Pin 						GPIO_Pin_1
#define LCD_PWR_GPIO_Port 					GPIOB
#define LCD_BKLT_Pin 						GPIO_Pin_8
#define LCD_BKLT_GPIO_Port 					GPIOA

#define BATRESNET							(2)
#define VCCRESNET							(1.666666666666667)
#define REFVOL								(3.301)

#define VCCvol   ((u16)(Get_Adc_Average(ADC_Channel_1, 10)*(REFVOL / 4096) * 1000 * VCCRESNET))
#define BATvol   ((u16)(Get_Adc_Average(ADC_Channel_2, 10)*(REFVOL / 4096) * 1000 * BATRESNET))

#define START_MODE_NONE		0
#define START_MODE_BAT		1
#define START_MODE_VCC		2

#define VCCTHR				4000
#define BATNOLOADTHR		3550
#define BATWITHLOADTHR		3500
/////////////////////////////////////////////////dna动画////////////////////////
//dna动画位置
#define dnaxstart 68
#define dnaystart 148
#define dnaxstop 172
#define dnaystop 178

#define dnaangle 320		//dna动画总角度
#define dnaspeed 60			//dna动画速度

#define xstep   1

#define max_node_number 8	//dna动画圆球数
#define node_radius 5		//dna动画圆球正常直径
#define node_change 0.3		//dna动画圆球变径指数

#define max_y_hight (dnaystop-dnaystart)
#define PII 3.14159265
#define y_frame   (max_y_hight - node_radius)
/////////////////////////////////////////////////dna动画end////////////////////////


#endif

u8 startmode;
u8 isstarting;
void MX_GPIO_Init(void);
void ICPX_GPIO_Init(void);
void ICPX_GPIO_Init(void);
void ICPX_Find_25Q80(void);
void ICPX_Test_25Q80(void);
void ICPX_BB_25Q80(u16 block);
void ICPX_Init_Spi_Bus(void);
void ICPX_Diplay_Image(u16 x, u16 y, typ_image_s inputimage);
u8 ICPX_Select_Font_Data(u16 num, u16 bytes, typ_fontlib_s fontlab);
void ICPX_Select_Font_Data_all(u8 *datas, u16 num, u16 numtoread, typ_fontlib_s fontlab);
u32 GetMCUID(void);
void ICPX_Standby(void);
void ICPX_Charge_Screen(u8 init);
void ICPX_Booting_Screen(u8 init);
void ICPX_Booting_Error_Screen(void);
void ICPX_DNA_CIRCLE(void);

void setback();
	
void MAINKEYTASK(void);
void CHGKEYTASK(void);
void MAINCHARGETASK(void);
void MAINBATCHECKTASK(void);
void STARTMODETASK(void);	
void SHUTDOWNMETH(u16 delay);
#endif // !__ICPX_BSP_H





