#ifndef __ICPX_IMAGE_H
#define __ICPX_IMAGE_H
#include "stm32f10x.h"

typedef struct
{
	u32	    StartADDR;
	u32		DataSize;
	u16		XSize;
	u16		YSize;
}typ_image_s;

typedef struct
{
	u32		StartADDR;
	u32		DataSize;
}typ_fontlib_s;


static typ_image_s ICOPYX_IMAGES[] = 
{
	//---------------------------------------------------------充电页面
#define BASE_ADDR (u32)0x0
//2 充电页 空@1x.ebm
//########### 0
	{
		.StartADDR = BASE_ADDR + (u32)0x0,  // 0x0
		.DataSize = 0x1c200,
		.XSize = 240,
		.YSize = 240,
	},
	//2-充电页-满格@1x_03.ebm
	//########### 1
	{
		.StartADDR = BASE_ADDR + (u32)0x1c200,  // 0x1c200
		.DataSize = 0x700,
		.XSize = 14,
		.YSize = 64,
	},
	//2-充电页-满格@1x_04.ebm
	//########### 2
	{
		.StartADDR = BASE_ADDR + (u32)0x1c900,  // 0x1c900
		.DataSize = 0x700,
		.XSize = 14,
		.YSize = 64,
	},
	//2-充电页-满格@1x_05.ebm
	//########### 3
	{
		.StartADDR = BASE_ADDR + (u32)0x1d000,  // 0x1d000
		.DataSize = 0x700,
		.XSize = 14,
		.YSize = 64,
	},
	//2-充电页-满格@1x_06.ebm
	//########### 4
	{
		.StartADDR = BASE_ADDR + (u32)0x1d700,  // 0x1d700
		.DataSize = 0x680,
		.XSize = 13,
		.YSize = 64,
	},
	//2-充电页-满格@1x_08.ebm
	//########### 5
	{
		.StartADDR = BASE_ADDR + (u32)0x1dd80,  // 0x1dd80
		.DataSize = 0x700,
		.XSize = 14,
		.YSize = 64,
	},
	//2-充电页-满格@1x_09.ebm
	//########### 6
	{
		.StartADDR = BASE_ADDR + (u32)0x1e480,  // 0x1e480
		.DataSize = 0x700,
		.XSize = 14,
		.YSize = 64,
	},
	//2-充电页-满格@1x_10.ebm
	//########### 7
	{
		.StartADDR = BASE_ADDR + (u32)0x1eb80,  // 0x1eb80
		.DataSize = 0x700,
		.XSize = 14,
		.YSize = 64,
	},
	//2-充电页-满格@1x_11.ebm
	//########### 8
	{
		.StartADDR = BASE_ADDR + (u32)0x1f280,  // 0x1f280
		.DataSize = 0x700,
		.XSize = 14,
		.YSize = 64,
	},
	//2-充电页-空@1x_03.ebm
	//########### 9
	{
		.StartADDR = BASE_ADDR + (u32)0x1f980,  // 0x1f980
		.DataSize = 0x38ac,
		.XSize = 117,
		.YSize = 62,
	},
#undef  BASE_ADDR

	//---------------------------------------------------------充电页面end
	//---------------------------------------------------------开机页面
#define BASE_ADDR (u32)0x0002322C
	//1 启动页@1x.ebm
	//########### 10
	{
		.StartADDR = BASE_ADDR + (u32)0x0,
		.DataSize = 0x1c200,
		.XSize = 240,
		.YSize = 240,
	},

	
	//---------------------------------------------------------开机页面end
};
#undef  BASE_ADDR
#define BASE_ADDR (u32)0x00050000
                      
static typ_fontlib_s ICOPYX_FONTLIBS[] = 
{
	//FONTLIB0
	//16x16
{
	.StartADDR = BASE_ADDR + (u32)0x00000000,
	.DataSize = 16,
},
	//FONTLIB1
	//32x32
{
	.StartADDR = BASE_ADDR + (u32)0x00005000,
	.DataSize = 64,
},
	//FONTLIB2
	//24x24
{
	.StartADDR = BASE_ADDR + (u32)0x00010000,
	.DataSize = 48,
},
	//FONTLIB3
	//20x20
{
	.StartADDR = BASE_ADDR + (u32)0x00015000,
	.DataSize = 40,
},
	//FONTLIB4(图标库）
	//20x20
{	//0x00160000
	.StartADDR = BASE_ADDR + (u32)0x00020000,
	.DataSize = 60,
}
		
};


#endif
