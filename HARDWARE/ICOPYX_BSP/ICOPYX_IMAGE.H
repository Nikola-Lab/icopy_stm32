#ifndef __ICPX_IMAGE_H
#define __ICPX_IMAGE_H
#include "stm32f10x.h"

typedef struct
{
	u8		id;		// 资源id
	u8		x;		//资源座标x		
	u8		y;		//资源座标y		
	u32	    StartADDR;
	u32		DataSize;
	u16		XSize;
	u16		YSize;
}typ_image_s;

typedef struct
{
	u8		id;		// 资源id
	u8		wordx;		//字体尺寸x		
	u8		wordy;		//字体尺寸y
	u32		StartADDR;
	u32		DataSize;
}typ_fontlib_s;

extern typ_image_s ICOPYX_IMAGES[20];
extern typ_fontlib_s ICOPYX_FONTLIBS[20];
extern int32_t ICOPYX_FW_VERSION;
extern int32_t ICOPYX_FW_SUBVERSION;
//检查数据库已经完整
u8 Check_img_lib();
//检查数据是否存在
u8 Check_img_data(u8 id);
u8 ICPX_img_data_ok;
#endif