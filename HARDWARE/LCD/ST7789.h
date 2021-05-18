#ifndef __ST7789_H
#define __ST7789_H		
#include "sys.h"



void ST7789_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);//指定区域填充颜色
void ST7789_FillTrans(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color, u8 trans);//填充带透明度的颜色
void ST7789_Read(u16 xsta, u16 ysta, u16 xend, u16 yend, u8* buf);//指定区域读出颜色


void ST7789_DrawPoint(u16 x,u16 y,u16 color);//在指定位置画一个点
void ST7789_DrawPointWithCache(u16 x, u16 y, u16 color, u8 flush);//在指定位置画一个点(带缓存)
void ST7789_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color);//在指定位置画一条线
void ST7789_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);//在指定位置画一个矩形
void ST7789_Draw_Circle(u16 x0,u16 y0,u8 r,u16 color);//在指定位置画一个圆
void ST7789_Draw_ALLCircle(u16 x0, u16 y0, u8 r, u16 color);//在指定位置画一个实心圆

void ST7789_ShowChinese(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示汉字串
void ST7789_ShowChinese16x16(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示单个16x16汉字
void ST7789_ShowChinese24x24(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示单个24x24汉字
void ST7789_ShowChinese32x32(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示单个32x32汉字

void ST7789_ShowChar(u16 x,u16 y,u8 num,u16 fc,u16 bc,u8 sizey,u8 mode);//显示一个字符
void ST7789_ShowSimbol(u16 x, u16 y, u8 num, u16 fc, u16 bc, u8 sizex, u8 sizey, u8 mode);//显示一个符号
void ST7789_ShowString(u16 x,u16 y,const u8 *p,u16 fc,u16 bc,u8 sizey,u8 mode);//显示字符串
u32 mypow(u8 m,u8 n);//求幂
void ST7789_ShowIntNum(u16 x,u16 y,u16 num,u8 len,u16 fc,u16 bc,u8 sizey);//显示整数变量
void ST7789_ShowFloatNum1(u16 x,u16 y,float num,u8 len,u16 fc,u16 bc,u8 sizey);//显示两位小数变量

void ST7789_ShowPicture(u16 x,u16 y,u16 length,u16 width,const u8 pic[]);//显示图片
void ST7789_SetBaklight(u8 light);


//画笔颜色
#define WHITE			0xFFFF
#define WHITE1			0xFFFE
#define BLACK			0x0000
#define BLACK1			0x0001 
#define BLUE			0x001F  
#define BRED			0XF81F
#define GRED 			0XFFE0
#define GBLUE			0X07FF
#define RED				0xF800
#define MAGENTA			0xF81F
#define GREEN			0x07E0
#define CYAN			0x7FFF
#define YELLOW			0xFFE0
#define BROWN			0XBC40//棕色
#define BRRED			0XFC07//棕红色
#define GRAY			0X8430//灰色
#define DARKBLUE		0X01CF//深蓝色
#define LIGHTBLUE		0X7D7C//浅蓝色
#define GRAYBLUE		0X5458//灰蓝色
#define LIGHTGREEN		0X841F//浅绿色
#define LGRAY			0XC618//浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE		0XA651//浅灰蓝色(中间层颜色)
#define LBBLUE			0X2B12//浅棕蓝色(选择条目的反色)
#define ICPX_BLUE_BAK   0x1b5d
#define ICPX_WHITE_FRONT 0XFFDF

#define YCLEAR 999

#endif





