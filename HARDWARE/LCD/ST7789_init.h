#ifndef __ST7789_INIT_H
#define __ST7789_INIT_H

#include "sys.h"
#include "ICOPYX_BSP.h"

#define USE_HORIZONTAL 3  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#define ST7789_W 240
#define ST7789_H 240



//-----------------LCD端口定义---------------- 

#define ST7789_RES_Clr()	GPIO_ResetBits(LCD_RST_GPIO_Port,LCD_RST_Pin)//RES
#define ST7789_RES_Set()	GPIO_SetBits(LCD_RST_GPIO_Port,LCD_RST_Pin)

#define ST7789_DC_Clr()		GPIO_ResetBits(LCD_DC_GPIO_Port,LCD_DC_Pin)//DC
#define ST7789_DC_Set()		GPIO_SetBits(LCD_DC_GPIO_Port,LCD_DC_Pin)

//#define ST7789_BL_OFF()		GPIO_ResetBits(LCD_BKLT_GPIO_Port,LCD_BKLT_Pin)//BLK
//#define ST7789_BL_ON()		GPIO_SetBits(LCD_BKLT_GPIO_Port,LCD_BKLT_Pin)

#define ST7789_BL_OFF()		ST7789_SetBaklight(0)
#define ST7789_BL_ON()		ST7789_SetBaklight(100)

#define ST7789_SELECT()		GPIO_ResetBits(LCD_CS_GPIO_Port,LCD_CS_Pin)//CS
#define ST7789_UNSELECT()   GPIO_SetBits(LCD_CS_GPIO_Port,LCD_CS_Pin)

#define ST7789_PWRON()		GPIO_ResetBits(LCD_PWR_GPIO_Port,LCD_PWR_Pin)//PWR
#define ST7789_PWROFF()		GPIO_SetBits(LCD_PWR_GPIO_Port,LCD_PWR_Pin)

void ST7789_GPIO_Init(void);//初始化GPIO
void ST7789_Writ_Bus(u8 dat);//模拟SPI时序
u8 ST7789_Read_Bus(u8 dat);//读取一个字节
void ST7789_Read_Busl(u8* buff, u16 num);//读取一个字节
void ST7789_WR_DATA8(u8 dat);//写入一个字节
void ST7789_WR_DATA(u16 dat);//写入两个字节
void ST7789_WR_DATA32(u32 dat);//写入四个字节
void ST7789_WR_DATAl(u8* dat,u16 num);//写入数个字节
void ST7789_WR_REG(u8 dat);//写入一个指令
void ST7789_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//设置坐标函数
void ST7789_Init(void);//LCD初始化
#endif




