#include "sys.h"
#include "ST7789_init.h"
#include "delay.h"
#include "spi.h"
#include "dma.h"
#include "ICOPYX_BSP.h"
#include "timer.h"

void ST7789_GPIO_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);	 //使能端口时钟
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
	
	//GPIO_InitStructure.GPIO_Pin = LCD_BKLT_Pin;
	//GPIO_Init(LCD_BKLT_GPIO_Port, &GPIO_InitStructure);
    //GPIO_ResetBits(LCD_BKLT_GPIO_Port, LCD_BKLT_Pin);
	
	GPIO_InitStructure.GPIO_Pin = LCD_DC_Pin;
	GPIO_Init(LCD_DC_GPIO_Port, &GPIO_InitStructure);
	GPIO_SetBits(LCD_DC_GPIO_Port, LCD_DC_Pin);
	
	GPIO_InitStructure.GPIO_Pin = LCD_CS_Pin;
	GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStructure);
	GPIO_SetBits(LCD_CS_GPIO_Port, LCD_CS_Pin);
	
	GPIO_InitStructure.GPIO_Pin = LCD_RST_Pin;
	GPIO_Init(LCD_RST_GPIO_Port, &GPIO_InitStructure);
	GPIO_SetBits(LCD_RST_GPIO_Port, LCD_RST_Pin);
	
	GPIO_InitStructure.GPIO_Pin = LCD_PWR_Pin;
	GPIO_Init(LCD_PWR_GPIO_Port, &GPIO_InitStructure);
	GPIO_SetBits(LCD_PWR_GPIO_Port, LCD_PWR_Pin);
}


void delay(int t)
{
    while(t--);
}
/******************************************************************************
      函数说明：LCD串行数据写入函数
      入口数据：dat  要写入的串行数据
      返回值：  无
******************************************************************************/
void ST7789_Writ_Bus(u8 dat)
{
	//while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);//检查接收标志位
    SPI_I2S_SendData(SPI1,dat);
    //delay(1);
}

/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void ST7789_WR_DATA8(u8 dat)
{
	ST7789_SELECT();
    ST7789_Writ_Bus(dat);
	ST7789_UNSELECT();
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void ST7789_WR_DATA(u16 dat)
{
	ST7789_SELECT();
    ST7789_Writ_Bus(dat>>8);
    ST7789_Writ_Bus(dat);
	ST7789_UNSELECT();
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void ST7789_WR_DATA32(u32 dat)
{
	ST7789_SELECT();
	ST7789_Writ_Bus(dat >> 24);
	ST7789_Writ_Bus(dat >> 16);
	ST7789_Writ_Bus(dat >> 8);
	ST7789_Writ_Bus(dat);
	ST7789_UNSELECT();
}
/******************************************************************************
      函数说明：LCD写入数个字节
      入口数据：dat 写入的数据,num写入数量
      返回值：  无
******************************************************************************/
void ST7789_WR_DATAl(u8* dat, u16 num)
{
	u16 i;
	ST7789_SELECT();
	for (i = 0;i < num;i++)
	{
		ST7789_Writ_Bus(dat[i]);
	}
	ST7789_UNSELECT();
}

/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
void ST7789_WR_REG(u8 dat)
{
	ST7789_SELECT();
    ST7789_DC_Clr();//写命令
    ST7789_Writ_Bus(dat);
    ST7789_DC_Set();//写数据
	ST7789_UNSELECT();
}


/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void ST7789_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{

	if(USE_HORIZONTAL==0)
    {
        ST7789_WR_REG(0x2a);//列地址设置
        ST7789_WR_DATA(x1);
        ST7789_WR_DATA(x2);
        ST7789_WR_REG(0x2b);//行地址设置
        ST7789_WR_DATA(y1);
        ST7789_WR_DATA(y2);
        ST7789_WR_REG(0x2c);//储存器写
    }
    else if(USE_HORIZONTAL==1)
    {
        ST7789_WR_REG(0x2a);//列地址设置
        ST7789_WR_DATA(x1);
        ST7789_WR_DATA(x2);
        ST7789_WR_REG(0x2b);//行地址设置
        ST7789_WR_DATA(y1+80);
        ST7789_WR_DATA(y2+80);
        ST7789_WR_REG(0x2c);//储存器写
    }
    else if(USE_HORIZONTAL==2)
    {
        ST7789_WR_REG(0x2a);//列地址设置
        ST7789_WR_DATA(x1);
        ST7789_WR_DATA(x2);
        ST7789_WR_REG(0x2b);//行地址设置
        ST7789_WR_DATA(y1);
        ST7789_WR_DATA(y2);
        ST7789_WR_REG(0x2c);//储存器写
    }
    else
    {
        ST7789_WR_REG(0x2a);//列地址设置
        ST7789_WR_DATA(x1+80);
        ST7789_WR_DATA(x2+80);
        ST7789_WR_REG(0x2b);//行地址设置
        ST7789_WR_DATA(y1);
        ST7789_WR_DATA(y2);
        ST7789_WR_REG(0x2c);//储存器写
    }
}

void ST7789_Init(void)
{
    SPI1_Init();
    ST7789_GPIO_Init();//初始化GPIO
	TIM1_PWM_Init(899, 0);	 //不分频。PWM频率=72000000/900=80Khz
	ST7789_SELECT();
	
	ST7789_PWRON();
	delay_ms(100);

    ST7789_RES_Clr();//复位
    delay_ms(100);
    ST7789_RES_Set();
    delay_ms(100);

//    ST7789_BL_ON();//打开背光
//    delay_ms(100);

    //************* Start Initial Sequence **********//
    ST7789_WR_REG(0x11); //Sleep out
    delay_ms(120);              //Delay 120ms
    //************* Start Initial Sequence **********//
    ST7789_WR_REG(0x36);
    if(USE_HORIZONTAL==0)ST7789_WR_DATA8(0x00);
    else if(USE_HORIZONTAL==1)ST7789_WR_DATA8(0xC0);
    else if(USE_HORIZONTAL==2)ST7789_WR_DATA8(0x70);
    else ST7789_WR_DATA8(0xA0);

    ST7789_WR_REG(0x3A);//传输格式定义
    ST7789_WR_DATA8(0x05);//00000101 101代表16bit单像素

    ST7789_WR_REG(0xB2);//前后肩设定
    ST7789_WR_DATA8(0x0C);
    ST7789_WR_DATA8(0x0C);
    ST7789_WR_DATA8(0x00);
    ST7789_WR_DATA8(0x33);
    ST7789_WR_DATA8(0x33);

    ST7789_WR_REG(0xB7);//VGH和VGL
    ST7789_WR_DATA8(0x35);//0X3+0X5 VGH=13.26V VGL=-10.43V 

    ST7789_WR_REG(0xBB);
    ST7789_WR_DATA8(0x19);

    ST7789_WR_REG(0xC0);
    ST7789_WR_DATA8(0x2C);

    ST7789_WR_REG(0xC2);
    ST7789_WR_DATA8(0x01);

    ST7789_WR_REG(0xC3);
    ST7789_WR_DATA8(0x12);

    ST7789_WR_REG(0xC4);
    ST7789_WR_DATA8(0x20);

    ST7789_WR_REG(0xC6);
    ST7789_WR_DATA8(0x09);

    ST7789_WR_REG(0xD0);
    ST7789_WR_DATA8(0xA4);
    ST7789_WR_DATA8(0xA1);

    ST7789_WR_REG(0xE0);
    ST7789_WR_DATA8(0xD0);
    ST7789_WR_DATA8(0x04);
    ST7789_WR_DATA8(0x0D);
    ST7789_WR_DATA8(0x11);
    ST7789_WR_DATA8(0x13);
    ST7789_WR_DATA8(0x2B);
    ST7789_WR_DATA8(0x3F);
    ST7789_WR_DATA8(0x54);
    ST7789_WR_DATA8(0x4C);
    ST7789_WR_DATA8(0x18);
    ST7789_WR_DATA8(0x0D);
    ST7789_WR_DATA8(0x0B);
    ST7789_WR_DATA8(0x1F);
    ST7789_WR_DATA8(0x23);

    ST7789_WR_REG(0xE1);
    ST7789_WR_DATA8(0xD0);
    ST7789_WR_DATA8(0x04);
    ST7789_WR_DATA8(0x0C);
    ST7789_WR_DATA8(0x11);
    ST7789_WR_DATA8(0x13);
    ST7789_WR_DATA8(0x2C);
    ST7789_WR_DATA8(0x3F);
    ST7789_WR_DATA8(0x44);
    ST7789_WR_DATA8(0x51);
    ST7789_WR_DATA8(0x2F);
    ST7789_WR_DATA8(0x1F);
    ST7789_WR_DATA8(0x1F);
    ST7789_WR_DATA8(0x20);
    ST7789_WR_DATA8(0x23);
    ST7789_WR_REG(0x21);

    ST7789_WR_REG(0x29);
}








