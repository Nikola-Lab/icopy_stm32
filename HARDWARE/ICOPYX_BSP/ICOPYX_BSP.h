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
#include "myiic.h"
#include "debugflag.h"
#include "rtc.h"
#include "AW87318.h"

#define ICOPYX_SW_VERSION (1)
#define ICOPYX_SW_SUBVERSION (0)

#define BLAddress 0x0800B400

///////////////////////////////////////////GPIO DEFINE START///////////////////			  
	#define WKUP_Pin 							GPIO_Pin_0
	#define WKUP_GPIO_Port 						GPIOA 

	#define VUSB_Pin  							GPIO_Pin_1
	#define VUSB_GPIO_Port 						GPIOA 

	#define VBAT_Pin 							GPIO_Pin_2
	#define VBAT_GPIO_Port 						GPIOA 

	#define BATSAMP_Pin 						GPIO_Pin_11
	#define BATSAMP_GPIO_Port 					GPIOB
			
	#define OTGSENSE_Pin 						GPIO_Pin_15
	#define OTGSENSE_GPIO_Port 					GPIOB

	#define CHARG_EN_Pin 						GPIO_Pin_12
	#define CHARG_EN_GPIO_Port 					GPIOB

	#define AMP_STDN_Pin						GPIO_Pin_6
	#define AMP_STDN_GPIO_Port					GPIOB	

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
	#define BUTM1_Pin 							GPIO_Pin_13
	#define BUTM1_GPIO_Port			 			GPIOA 
	#define BUTM2_Pin 							GPIO_Pin_11
	#define BUTM2_GPIO_Port 					GPIOA 
	#define BUTPWR_CAN_Pin 						GPIO_Pin_14
	#define BUTPWR_CAN_GPIO_Port 				GPIOA 
	#define BUTALL_Pin 							GPIO_Pin_10
	#define BUTALL_GPIO_Port 					GPIOB 
												  
	#define H3_PWR_ON_OFF_Pin 					GPIO_Pin_8
	#define H3_PWR_ON_OFF_GPIO_Port 			GPIOB 
	#define PM_PWR_ON_OFF_Pin 					GPIO_Pin_9
	#define PM_PWR_ON_OFF_GPIO_Port 			GPIOB 

	#define Q25_EN_Pin 						    GPIO_Pin_2
	#define Q25_EN_GPIO_Port 					GPIOB 

	#define VUSB_Pin  							GPIO_Pin_1
	#define VUSB_GPIO_Port 						GPIOA 
												  
	#define PM_BUTTON_Pin 						GPIO_Pin_7
	#define PM_BUTTON_GPIO_Port 				GPIOB
												  
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

	#define BATRESNET							(1.333333333333333)
	#define VCCRESNET							(1.666666666666667)
	#define REFVOL								(3.301)
	#define INTREFVOL							(1.200)

	#define Intvolavl	((u16)(Get_Adc_Average(ADC_Channel_17, 1)))

		//#define VCCvolavlNOSE   ((u16)(Get_Adc_Average(ADC_Channel_1, 2)*(REFVOL / 4096) * 1000 * VCCRESNET))
		//#define BATvolavlNOSE   ((u16)(Get_Adc_Average(ADC_Channel_2, 2)*(REFVOL / 4096) * 1000 * BATRESNET))
	#define VCCvolavl   ((u16)(Get_Adc_Average(ADC_Channel_1, 2)*(INTREFVOL / Intvolavl) * 1000 * VCCRESNET))
	#define BATvolavl   ((u16)(Get_Adc_Average(ADC_Channel_2, 100)*(INTREFVOL / Intvolavl) * 1000 * BATRESNET))

		//#define VCCvolNOSE   ((u16)(Get_Adc(1)*(REFVOL / 4096) * 1000 * VCCRESNET))
		//#define BATvolNOSE   ((u16)(Get_Adc(2)*(REFVOL / 4096) * 1000 * BATRESNET))
	#define VCCvol   ((u16)(Get_Adc(1)*(INTREFVOL / Intvolavl) * 1000 * VCCRESNET))
		//#define BATvol   ((u16)(Get_Adc(2)*(INTREFVOL / Intvolavl) * 1000 * BATRESNET))

	#define turnoffpm3()	GPIO_SetBits(PM_PWR_ON_OFF_GPIO_Port, PM_PWR_ON_OFF_Pin)
	#define turnoffh3()		GPIO_SetBits(H3_PWR_ON_OFF_GPIO_Port, H3_PWR_ON_OFF_Pin)
	#define turnonpm3()		GPIO_ResetBits(PM_PWR_ON_OFF_GPIO_Port, PM_PWR_ON_OFF_Pin)
	#define turnonh3()		GPIO_ResetBits(H3_PWR_ON_OFF_GPIO_Port, H3_PWR_ON_OFF_Pin)

	#define turnonchg()		GPIO_ResetBits(CHARG_EN_GPIO_Port, CHARG_EN_Pin)
	#define turnoffchg()	GPIO_SetBits(CHARG_EN_GPIO_Port, CHARG_EN_Pin)
	#define readchg()       GPIO_ReadOutputDataBit(CHARG_EN_GPIO_Port, CHARG_EN_Pin)

	#define SPISELH3()		GPIO_SetBits(SPI_SEL_GPIO_Port, SPI_SEL_Pin)
	#define SPISELST()		GPIO_ResetBits(SPI_SEL_GPIO_Port, SPI_SEL_Pin)



#define START_MODE_NONE		0
#define START_MODE_BAT		1
#define START_MODE_VCC		2

#define VCCTHRHIGH			3500//4100
#define VCCTHRLOW			2000//4000
#define BATNOLOADTHR		3600
#define NOBATTHR			1000
#define BATWITHLOADTHR		3400
/////////////////////////////////////////////////dna动画////////////////////////
//dna动画位置
#define dnaxstart 68
#define dnaystart 148
#define dnaxstop 172
#define dnaystop 178

#define dnaangle 320		//dna动画总角度
#define dnaspeed 10			//dna动画速度

#define xstep   1

#define max_node_number 8	//dna动画圆球数
#define node_radius 5		//dna动画圆球正常直径
#define node_change 0.3		//dna动画圆球变径指数

#define max_y_hight (dnaystop-dnaystart)
#define PII 3.14159265
#define y_frame   (max_y_hight - node_radius)
/////////////////////////////////////////////////dna动画end////////////////////////

u8 startmode;
u8 isstarting;
u8 testkeying;
u8 stdnflag;
void MX_GPIO_Init(void);
void ICPX_GPIO_Init(void);
void ICPX_AMP_Init(void);
void ICPX_BKP_Init(void);
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
void ICPX_Shutdown_Screen(u8 init);
void ICPX_Booting_Error_Screen(u8 init);
void ICPX_DNA_CIRCLE(void);
void ICPX_write_file_addr_cache(u8 id, u32 addr);
void ICPX_write_file_para_cache(u8 id, u8 PARAS, u8 length, u8* datas);
u16 ICPX_BAT_VOL_GATHER(u8 MODE);
u16 ICPX_BAT_VOL_REVICE(u8 what);
u16 ICPX_BAT_VOL_REQUEST(u8 what, u8 Step);

void setback();
u32 map(u32 x, u32 in_min, u32 in_max, u32 out_min, u32 out_max);
	
void MAINKEYTASK(void);
void CHGKEYTASK(u8 en);
u8 MAINCHARGETASK(u8 what);
u32 BATVOL2PERCENT(u16 VOL);
u16 MAINBATCHECKTASK(u8 what);
void STARTMODETASK(void);	
void SHUTDOWNMETH();
void SETSHUTDOWNFLAG();
void CHARGE_OTG();
#endif // !__ICPX_BSP_H
