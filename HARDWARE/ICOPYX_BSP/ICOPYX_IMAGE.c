#include "ICOPYX_IMAGE.H"

u8 ICPX_img_data_ok = 0;
typ_image_s ICOPYX_IMAGES[20] = 
{
	//---------------------------------------------------------充电页面
	//充电页 空@1x
{
	.id = 1,
	.x = 0,	
	.y = 0,
	.StartADDR = 0,
	.DataSize = 0,
	.XSize = 0,
	.YSize = 0,	
},
	//充电页-满格@1x_03
{
	.id = 2,
	.x = 0,	
	.y = 0,
	.StartADDR = 0,
	.DataSize = 0,
	.XSize = 0,
	.YSize = 0,	
},
	//充电页-满格@1x_04
{
	.id = 3,
	.x = 0,	
	.y = 0,
	.StartADDR = 0,
	.DataSize = 0,
	.XSize = 0,
	.YSize = 0,	
},
	//充电页-满格@1x_05
{
	.id = 4,
	.x = 0,	
	.y = 0,
	.StartADDR = 0,
	.DataSize = 0,
	.XSize = 0,
	.YSize = 0,	
},
	//充电页-满格@1x_06
{
	.id = 5,
	.x = 0,	
	.y = 0,
	.StartADDR = 0,
	.DataSize = 0,
	.XSize = 0,
	.YSize = 0,	
},
	//充电页-满格@1x_08
{
	.id = 6,
	.x = 0,	
	.y = 0,
	.StartADDR = 0,
	.DataSize = 0,
	.XSize = 0,
	.YSize = 0,	
},
	//充电页-满格@1x_09
{
	.id = 7,
	.x = 0,	
	.y = 0,
	.StartADDR = 0,
	.DataSize = 0,
	.XSize = 0,
	.YSize = 0,	
},
	//满格@1x_10
{
	.id = 8,
	.x = 0,	
	.y = 0,
	.StartADDR = 0,
	.DataSize = 0,
	.XSize = 0,
	.YSize = 0,	
},
	//满格@1x_11
{
	.id = 9,
	.x = 0,	
	.y = 0,
	.StartADDR = 0,
	.DataSize = 0,
	.XSize = 0,
	.YSize = 0,	
},
	//充电页-空@1x_03
{
	.id = 10,
	.x = 0,	
	.y = 0,
	.StartADDR = 0,
	.DataSize = 0,
	.XSize = 0,
	.YSize = 0,	
},

	//---------------------------------------------------------充电页面end
	//---------------------------------------------------------开机页面
	//启动页
{
	.id = 11,
	.x = 0,	
	.y = 0,
	.StartADDR = 0,
	.DataSize = 0,
	.XSize = 0,
	.YSize = 0,	
},
	//---------------------------------------------------------开机页面end
{
	//12-充电页满格@1x_03（绿）
	.id = 12,
	.x = 0,	
	.y = 0,
	.StartADDR = 0,
	.DataSize = 0,
	.XSize = 0,
	.YSize = 0,	
},
};
typ_fontlib_s ICOPYX_FONTLIBS[20] = 
{
	//FONTLIB0
	//16x16
{
	.id = 101,
	.wordx = 0,		
	.wordy = 0,
	.StartADDR = 0,
	.DataSize = 0,
},
	//FONTLIB1
	//20x20
{
	.id = 102,
	.wordx = 0,		
	.wordy = 0,
	.StartADDR = 0,
	.DataSize = 0,
},
	//FONTLIB2
	//24x24
{
	.id = 103,
	.wordx = 0,		
	.wordy = 0,
	.StartADDR = 0,
	.DataSize = 0,
},
	//FONTLIB3
	//32x32
{
	.id = 104,
	.wordx = 0,		
	.wordy = 0,
	.StartADDR = 0,
	.DataSize = 0,
}
		
};
int32_t ICOPYX_FW_VERSION = 0;
int32_t ICOPYX_FW_SUBVERSION = 0;
//检测关键数据是否为有效数据，返回1为正常
u8 Check_img_lib()
{
	u8 id;
	for (id = 1;id < 13;id++)
	{
		if (Check_img_data(id) != 1)
			return 0;
	}
	for (id = 101;id < 105;id++)
	{
		if (Check_img_data(id) != 1)
			return 0;
	}
	return 1;
}

//检查数据是否完整，传入ID，检测关键数据是否为有效数据，返回1为正常
u8 Check_img_data(u8 id)
{
	if (id > 100)
	{
		int i;
		//字体库
		if(	ICOPYX_FONTLIBS[id-101].DataSize != 0&&
			ICOPYX_FONTLIBS[id-101].StartADDR != 0&&
			ICOPYX_FONTLIBS[id-101].wordx != 0&&
			ICOPYX_FONTLIBS[id-101].wordy != 0)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		if (ICOPYX_IMAGES[id-1].DataSize != 0&&
			ICOPYX_IMAGES[id-1].StartADDR != 0&&
			ICOPYX_IMAGES[id-1].XSize != 0&&
			ICOPYX_IMAGES[id-1].YSize != 0)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	
}

