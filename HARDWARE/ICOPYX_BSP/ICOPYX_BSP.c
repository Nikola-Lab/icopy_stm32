#include "ICOPYX_BSP.h"

u8 startmode = START_MODE_NONE;
u8 isstarting = 1;
u8 hasbak = 0;
//板载gpio初始化
void ICPX_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//使能A端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//使能B端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);		//使能C端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);		//使能D端口时钟

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//使能复用网络时钟

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//关闭jlink，打开SWJ
	GPIO_PinRemapConfig(GPIO_Remap_PD01, ENABLE);				//使能PD0和1复用

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_InitStructure.GPIO_Pin = PM_BUTTON_Pin;				//PM3按钮
	GPIO_Init(PM_BUTTON_GPIO_Port, &GPIO_InitStructure);
	GPIO_ResetBits(PM_BUTTON_GPIO_Port, PM_BUTTON_Pin);

	GPIO_InitStructure.GPIO_Pin = SPI_SEL_Pin;					//SPI切换开关
	GPIO_Init(SPI_SEL_GPIO_Port, &GPIO_InitStructure);
	GPIO_ResetBits(SPI_SEL_GPIO_Port, SPI_SEL_Pin);

	GPIO_InitStructure.GPIO_Pin = FLASH_PWR_Pin;				//Flash电源引脚
	GPIO_Init(FLASH_PWR_GPIO_Port, &GPIO_InitStructure);
	GPIO_SetBits(FLASH_PWR_GPIO_Port, FLASH_PWR_Pin);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;

	GPIO_InitStructure.GPIO_Pin = H3_PWR_ON_OFF_Pin;
	GPIO_Init(H3_PWR_ON_OFF_GPIO_Port, &GPIO_InitStructure);
	GPIO_SetBits(H3_PWR_ON_OFF_GPIO_Port, H3_PWR_ON_OFF_Pin);

	GPIO_InitStructure.GPIO_Pin = PM_PWR_ON_OFF_Pin;
	GPIO_Init(PM_PWR_ON_OFF_GPIO_Port, &GPIO_InitStructure);
	GPIO_SetBits(PM_PWR_ON_OFF_GPIO_Port, PM_PWR_ON_OFF_Pin);

	GPIO_InitStructure.GPIO_Pin = CHARG_EN_Pin;
	GPIO_Init(CHARG_EN_GPIO_Port, &GPIO_InitStructure);
	GPIO_ResetBits(CHARG_EN_GPIO_Port, CHARG_EN_Pin);
	
	//PD1和0开漏输出或复用开漏输出，不可设置为推挽输出或复用推挽输出
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;					//D0用于拉低电池采集引脚电压
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOD, GPIO_Pin_0);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//浮空输入
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;					//D1用于检测otg输出状态
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;				//输入，开启下拉

	GPIO_InitStructure.GPIO_Pin = PM_4LED_Pin;
	GPIO_Init(PM_4LED_GPIO_Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = CHARG_STATE_Pin;
	GPIO_Init(CHARG_STATE_GPIO_Port, &GPIO_InitStructure);

}
void ICPX_Find_25Q80(void)
{
	u8 timesout = 0;
	while (W25QXX_ReadID() != W25Q80 || timesout >= 15)//检测不到W25Q128
	{
		printf("\r\nW25QXX Check Failed!  ");
		delay_ms(100);
		printf(" Please Check! ID = %X\r\n", W25QXX_ReadID());
		delay_ms(100);
		timesout += 1;
	}
	if (timesout >= 15)
	{
		printf("\r\nW25QXX Error!\r\n");
	}
	else
	{
		printf("\r\nW25QXX OK!\r\n");
	}

}
//25Q80测试代码
void ICPX_Test_25Q80(void)
{
	u8 datatemp[20];
	u8 dataprint[20];
	u8 i;
	memset(datatemp, 0, sizeof(datatemp));
	memset(dataprint, 0, sizeof(dataprint));
	printf("W25Qxx ID:%02X\r\n", W25QXX_TYPE);
	W25QXX_ReadUID(datatemp);
	sprintf(dataprint, "0x%02X%02X%02X%02X%02X%02X%02X%02X", datatemp[0], datatemp[1], datatemp[2], datatemp[3], datatemp[4], datatemp[5], datatemp[6], datatemp[7]);
	printf("W25Qxx UID:%s\r\n", dataprint);
	printf("readdata:");
	for (i = 0; i < 128; i++)
	{
		memset(datatemp, 0, sizeof(datatemp));
		memset(dataprint, 0, sizeof(dataprint));
		W25QXX_Read(datatemp, 0x0001C200 + i, 1);
		sprintf(dataprint, "0x%02X,", datatemp[0]);
		printf("%s", dataprint);
	}
	printf("\r\n");
	fflush(stdout);
//	u8 testdata[5120];
//	memset(testdata, 0, sizeof(testdata));
//	testdata[0] = 0x02;
//
//	W25QXX_Write(testdata, 0x00000000, 5120);
//	//W25QXX_Write(testdata, 0x00036800, 5120);
//
//	printf("readdata:");
//	for (i = 0;i < 128;i++)
//	{
//		memset(datatemp, 0, sizeof(datatemp));
//		memset(dataprint, 0, sizeof(dataprint));
//		W25QXX_Read(datatemp, 0x00000000 + i, 1);
//		sprintf(dataprint, "0x%02X,", datatemp[0]);
//		printf("%s", dataprint);
//	}
//	fflush(stdout);
}
//25Q80坏快测试
void ICPX_BB_25Q80(u16 block)
{
	u8 datatemp[20];
	u8 dataprint[20];
	u32 i = 0;
	u32 j = 0;
	u8 teststr[] = "12345678";
	while (1)
	{
		for (i = 0; i < 0x00400000; i += 0x10000)
		{
			W25QXX_Erase_Sector(i / 4096);
			W25QXX_Write_NoCheck(teststr, i, 8);//写入15byte
			memset(datatemp, 0, sizeof(datatemp));
			memset(dataprint, 0, sizeof(dataprint));
			W25QXX_Read(datatemp, i, (u32)8);
			sprintf(dataprint, "0x%02X%02X%02X%02X%02X%02X%02X%02X", datatemp[0], datatemp[1], datatemp[2], datatemp[3], datatemp[4], datatemp[5], datatemp[6], datatemp[7]);
			printf("    readdata:%s\r\n", dataprint);
			if (strcmp(datatemp, teststr) == 0)
			{
				printf("    dataok\r\n");
			}
			else
			{
				printf("    dataerr\r\n");
				break;
			}
		}
		printf("times:%d\r\n", j++);
	}

}
//lcd和spiflash初始化（包含spi初始化）
void ICPX_Init_Spi_Bus(void)
{
	ST7789_Init();			//LCD初始化
	W25QXX_Init();			//W25QXX初始化
}
void ICPX_Diplay_Image(u16 x, u16 y, typ_image_s inputimage)
{

	u16 Y = 0;

	//20200728 添加多行刷新，加速刷图
	//u8  LineData[inputimage.XSize * 2 + 1];

	u32 curaddr;
	curaddr = inputimage.StartADDR;

	//20200728 添加多行刷新，加速刷图
	u8 blocklines = 15;//刷图每次处理的行数，很影响速度
	u8 blocks = inputimage.YSize / blocklines;//记录要刷的次数
	u8 endblocklines = inputimage.YSize % blocklines;//记录最后一块行数（不满足一块的尺寸）
	u8 i;
	u8 LineData[inputimage.XSize * 2 * blocklines + 1];

	for (i = 0; i < blocks; i++) //刷整块
	{
		W25QXX_FastRead(LineData, curaddr, inputimage.XSize * 2 * blocklines);
		curaddr += inputimage.XSize * 2 * blocklines;
		ST7789_ShowPicture(x, y + Y, inputimage.XSize, blocklines, LineData);
//		printf("0x%02x", LineData[0]);
//		printf(",");
//		printf("0x%02x", LineData[1]);
//		printf(",");
//		printf("0x%02x", LineData[2]);
//		printf(",\r\n");
		Y += blocklines;
	}

	W25QXX_FastRead(LineData, curaddr, inputimage.XSize * 2 * endblocklines);
	curaddr += inputimage.XSize * 2 * endblocklines;
	if (Y < 240)
	{
		ST7789_ShowPicture(x, y + Y, inputimage.XSize, endblocklines, LineData);
	}

	//20200728 添加多行刷新，加速刷图 end

//	for (Y = 0;Y < inputimage.Ysize;Y++)
//	{
//		W25QXX_Read(LineData, curaddr, inputimage.XSize * 2);
//		curaddr += inputimage.XSize * 2;
//		ST7789_ShowPicture(x, y + Y, inputimage.XSize, 1, LineData);
//	}

}
u8 ICPX_Select_Font_Data(u16 num, u16 bytes, typ_fontlib_s fontlab)
{
	u32 aimbaseaddr = fontlab.StartADDR + num * fontlab.DataSize;
	u32 readaaddr = aimbaseaddr + bytes;
	u8 fontdata;
	W25QXX_FastRead(&fontdata, readaaddr, 1);
	return fontdata;
}
void ICPX_Select_Font_Data_all(u8* datas, u16 num, u16 numtoread, typ_fontlib_s fontlab)
{
	u32 aimbaseaddr = fontlab.StartADDR + num * fontlab.DataSize;
	W25QXX_FastRead(datas, aimbaseaddr, numtoread);
	return ;
}
u8 ICPX_find_RES_by_id(u8 id)
{
	u8 i;
	if (id > 100)
	{
		for (i = 0; i < 4; i++)
		{
			if (ICOPYX_FONTLIBS[i].id == id)
			{
				return i;
			}
		}
	}
	else
	{
		for (i = 0 ; i < 12 ; i++)
		{
			if (ICOPYX_IMAGES[i].id == id)
			{
				return i;
			}
		}
	}
	return 255;
}
//写入文件目标地址到文件缓存
void ICPX_write_file_addr_cache(u8 id, u32 addr)
{
	u8 pos = ICPX_find_RES_by_id(id);
	if (pos == 255)
	{
		return;
	}
	if (id > 100)
	{
		ICOPYX_FONTLIBS[pos].StartADDR = addr;
	}
	else
	{
		ICOPYX_IMAGES[pos].StartADDR = addr;
	}
}
//写入参数数据到文件缓存
void ICPX_write_file_para_cache(u8 id, u8 PARAS, u8 length, u8* datas)
{
	u8 pos = ICPX_find_RES_by_id(id);
	if (pos == 255)
	{
		return;
	}
	u8 i;
	u32 datatemp = 0;

	for (i = 0; i < length; i++)
	{
		datatemp += datas[i] << ((length - 1 - i) * 8);
	}


	if (id > 100)
	{
		//FONTSIZE		0x76
		//FONTX			0x77
		//FONTY			0x78
		switch (PARAS)
		{
		case 0x76:
			ICOPYX_FONTLIBS[pos].DataSize = datatemp;
			break;
		case 0x77:
			ICOPYX_FONTLIBS[pos].wordx = datatemp;
			break;
		case 0x78:
			ICOPYX_FONTLIBS[pos].wordy = datatemp;
			break;
		}

	}
	else
	{
		//PICW			0x71
		//PICH			0x72
		//PICX			0x73
		//PICY			0x74
		//PICSIZE		0x75
		switch (PARAS)
		{
		case 0x71:
			ICOPYX_IMAGES[pos].XSize = datatemp;
			break;
		case 0x72:
			ICOPYX_IMAGES[pos].YSize = datatemp;
			break;
		case 0x73:
			ICOPYX_IMAGES[pos].x = datatemp;
			break;
		case 0x74:
			ICOPYX_IMAGES[pos].y = datatemp;
			break;
		case 0x75:
			ICOPYX_IMAGES[pos].DataSize = datatemp;
			break;
		}
	}

}




////////////////////////////////////////////////////////////////////   页面相关   ////////////////////////
void ICPX_Charge_Screen(u8 init)
{
	static u8 firstshow = 0;
	static u8 showpic = 1;//1-9
	if (init)
	{
		firstshow = 0;
		showpic = 1;
		return;
	}
	if (!firstshow)
	{
		ICPX_Diplay_Image(0, 0, ICOPYX_IMAGES[0]);
		CHGKEYTASK(0);
		ST7789_BL_ON();
		firstshow = 1;
		g_Tim2Array[eTim2] = 0;
	}
	if (MAINBATCHECKTASK(1) > 4190)
	{
		ICPX_Diplay_Image(ICOPYX_IMAGES[11].x, ICOPYX_IMAGES[11].y, ICOPYX_IMAGES[11]);
		return;
	}
	if (g_Tim2Array[eTim2] > 500)
	{
		ICPX_Diplay_Image(ICOPYX_IMAGES[showpic].x, ICOPYX_IMAGES[showpic].y, ICOPYX_IMAGES[showpic]);
		CHGKEYTASK(0);
		showpic++;
		if (showpic > 9)
		{
			showpic = 1;
		}
		g_Tim2Array[eTim2] = 0;
	}
}
void ICPX_Booting_Screen(u8 init)
{
	static u8 firstshow = 0;
	if (init)
	{
		firstshow = 0;
		return;
	}
	if (!firstshow)
	{
		g_Tim2Array[eTim2] = 0;
		ST7789_BL_ON();
		while (IS_TIMEOUT_1MS(eTim2, 500))
		{
			//ST7789_Fill(0, 0, ST7789_H, ST7789_W, GREEN);
			ICPX_Diplay_Image(0, 0, ICOPYX_IMAGES[10]);
		}
		firstshow = 1;
	}
	ST7789_ShowString(60, 200, "Booting...", WHITE, BLACK, 24, 1);
	g_Tim2Array[eTim2] = 0;
	while (IS_TIMEOUT_1MS(eTim2, 500))
	{
		ICPX_DNA_CIRCLE();
	}
}
void ICPX_Shutdown_Screen(u8 init)
{
	static u8 firstshow = 0;
	if (init)
	{
		firstshow = 0;
		return;
	}
	if (!firstshow)
	{
		g_Tim2Array[eTim2] = 0;
		while (IS_TIMEOUT_1MS(eTim2, 50))
		{
			//ST7789_Fill(0, 0, 240, 240, ICPX_BLUE_BAK);
			ICPX_Diplay_Image(0, 0, ICOPYX_IMAGES[10]);
			ST7789_ShowString(54, 176, "Shutingdown", WHITE, ICPX_BLUE_BAK, 24, 0);
			ST7789_BL_ON();
		}
		firstshow = 1;
	}
	else
	{
		// 非第一次显示
		//g_Tim2Array[eTim2] = 0;
		//while (IS_TIMEOUT_1MS(eTim2, 50))
		//{
		//	ST7789_Fill(0, 0, 240, 240, BLUE);
		//}

	}
	g_Tim2Array[eTim2] = 0;
	while (IS_TIMEOUT_1MS(eTim2, 100))
	{
		ST7789_ShowString(84, 200, ".   ..", YELLOW, ICPX_BLUE_BAK, 24, 0);
	}
	g_Tim2Array[eTim2] = 0;
	while (IS_TIMEOUT_1MS(eTim2, 100))
	{
		ST7789_ShowString(84, 200, "..   .", YELLOW, ICPX_BLUE_BAK, 24, 0);
	}
	g_Tim2Array[eTim2] = 0;
	while (IS_TIMEOUT_1MS(eTim2, 100))
	{
		ST7789_ShowString(84, 200, "...   ", YELLOW, ICPX_BLUE_BAK, 24, 0);
	}
	g_Tim2Array[eTim2] = 0;
	while (IS_TIMEOUT_1MS(eTim2, 100))
	{
		ST7789_ShowString(84, 200, " ...  ", YELLOW, ICPX_BLUE_BAK, 24, 0);
	}
	g_Tim2Array[eTim2] = 0;
	while (IS_TIMEOUT_1MS(eTim2, 100))
	{
		ST7789_ShowString(84, 200, "  ... ", YELLOW, ICPX_BLUE_BAK, 24, 0);
	}
	g_Tim2Array[eTim2] = 0;
	while (IS_TIMEOUT_1MS(eTim2, 100))
	{
		ST7789_ShowString(84, 200, "   ...", YELLOW, ICPX_BLUE_BAK, 24, 0);
	}
}
void ICPX_Booting_Error_Screen(u8 init)
{
	static u8 firstshow = 0;
	if (init)
	{
		firstshow = 0;
		return;
	}
	if (!firstshow)
	{
		ICPX_Diplay_Image(0, 0, ICOPYX_IMAGES[10]);
		firstshow = 1;
	}
	g_Tim2Array[eTim3] = 0;
	while (IS_TIMEOUT_1MS(eTim3, 300)) ;
	ST7789_ShowString(18, 180, "Boot timeout!", YELLOW, ICPX_BLUE_BAK, 32, 0);
	g_Tim2Array[eTim3] = 0;
	while (IS_TIMEOUT_1MS(eTim3, 300)) ;
	ST7789_ShowString(18, 180, "             ", YELLOW, ICPX_BLUE_BAK, 32, 0);
}
////////////////////////////////////////////////////////////////////   页面相关END   /////////////////////



////////////////////////////////////////////////////////////////////   动画相关   ////////////////////////
double tsin(double x)
{
	double g = 0; //g为sinx()最终结果
	double t = x; //初始值x赋给t
	int n = 1; //n 用于求分母
	do
	{
		g += t;
		n++;
		t = -t * x * x / (2 * n - 1) / (2 * n - 2);
	}
	while (fabs(t) >= 1e-6) ;  //fabs()求double型变量的绝对值
	return g;
}
double tcos(double x)
{
	x += PII;

	if (x > PII)
	{
		x -= 2 * PII;
	}

	return (tsin(x));
}
void ICPX_DNA_CIRCLE(void)
{
	int node = 0;
	int x = 0;//度数
	static s16 frame = 0;
	//frame记录当前帧，一个周期内

	//准备sin和cos表
	static s16 sinb[360];
	static s16 cosb[360];
	static s8 sizesinb[360];
	static s8 sizecosb[360];
	static s8 needbl = 1;
	static s8 nodes[max_node_number][4];
	static s8 nodeslast[max_node_number][4];
	if (needbl == 1)
	{
		for (node = 0; node < 360; node++)
		{
			sinb[node] = ((tsin(x * PII / 180) * 1000) + 1000) * y_frame / (2000);
			sizesinb[node] = tsin((x - 90) % 360 * PII / 180) * 2 * node_change * node_radius;
			cosb[node] = ((tcos(x * PII / 180) * 1000) + 1000) * y_frame / (2000);
			sizecosb[node] = tcos((x - 90) % 360 * PII / 180) * 2 * node_change * node_radius;
			x += xstep;
		}
		//表构建完成
		needbl = 0;
	}
	//表准备结束，上面代码不会再运行了


	static u16 aa = (max_node_number - 1);
	//目标node数量
	static u16 bb;
	bb = dnaangle / aa;
	//相邻node的间距角度
	static u16 cc;
	cc = frame * dnaspeed;// / aa;
	//当前帧数x速度也就是目标角度
	static u8 zerotime = 0;

	//开始查表运算目标点位置
	for (node = 0 ; node < max_node_number ; node++)
	{
		//循环每个节点
		int aimangle = (node * bb + cc) % 360;
		nodes[node][0] = sinb[aimangle];
		nodes[node][1] = sizesinb[aimangle];
		nodes[node][2] = cosb[aimangle];
		nodes[node][3] = sizecosb[aimangle];
	}
	//过零检测
	if (cc % 360 ==  0)
	{
		zerotime++;
	}
	//检测一周期完成
	if (zerotime == 1)
	{
		frame = 0;
		zerotime = 0;
	}
	//下一帧
	frame++;
	static int fstnodex = dnaxstart + (node_radius + (node_change * node_radius)) / 2;
	static int nodestep = (dnaxstop - dnaxstart - (node_radius + (node_change * node_radius))) / (max_node_number - 1);
	static int halfheight = (dnaystart + dnaystop) / 2;

	//显示当前帧数据
	for (node = 0; node < max_node_number; node++)
	{
		ST7789_Draw_ALLCircle((u16)(fstnodex + node * nodestep), nodeslast[node][0] + halfheight	, (u8)((nodeslast[node][1] + node_radius) / 2)	, ICPX_BLUE_BAK);
		ST7789_Draw_ALLCircle((u16)(fstnodex + node * nodestep), nodes[node][0] + halfheight		, (u8)((nodes[node][1] + node_radius) / 2)		, WHITE);
		ST7789_Draw_ALLCircle((u16)(fstnodex + node * nodestep), nodeslast[node][2] + halfheight	, (u8)((nodeslast[node][3] + node_radius) / 2)	, ICPX_BLUE_BAK);
		ST7789_Draw_ALLCircle((u16)(fstnodex + node * nodestep), nodes[node][2] + halfheight		, (u8)((nodes[node][3] + node_radius) / 2)		, 0xDEFB);
	}
	//保存当前帧数据到上一帧寄存器
	for (node = 0; node < max_node_number; node++)
	{
		//循环每个节点
		nodeslast[node][0] = nodes[node][0];
		nodeslast[node][1] = nodes[node][1];
		nodeslast[node][2] = nodes[node][2];
		nodeslast[node][3] = nodes[node][3];
	}
	return;
}
////////////////////////////////////////////////////////////////////   动画相关END   /////////////////////


////////////////////////////////////////////////////////////////////   按键相关   ////////////////////////
//主扫描按键流程
void MAINKEYTASK(void)
{
	//长按标志位，代表当前周期已经是长按期间需要处理连按

	static u8 uplongpress = 0;
	static u8 downlongpress = 0;
	static u8 leftlongpress = 0;
	static u8 rightlongpress = 0;

	u8 key = 0;
	if (KEY_POWER(0) > 3000)
	{
		KEY_POWER(1);
		if (isstarting != 1)
		{
			SHUTDOWNMETH();//关机指令
		}
	}
	//---------------------------------------------上键
	//长按处理
	if (uplongpress == 1)
	{
		//长按超过1000ms后置位，进入长按处理
		if (KEY_ICPY_UP(0) == 0)
		{
			//按键抬起
			KEY_ICPY_UP(1);
			uplongpress = 0;
		}
		if (KEY_ICPY_UP(0) > 100)
		{
			//按键长按超过100ms
			KEY_ICPY_UP(1);
			printf("KEYUP_PRES!\r\n");
			fflush(stdout);
		}

	}
	if (KEY_ICPY_UP(0) > 1000 && uplongpress == 0)
	{
		//长按超过1000ms且长按标置位未置位
		KEY_ICPY_UP(1);
		uplongpress = 1;
	}
	//---------------------------------------------下键
	//长按处理
	if (downlongpress == 1)
	{
		//长按超过1000ms后置位，进入长按处理
		if (KEY_ICPY_DOWN(0) == 0)
		{
			//按键抬起
			KEY_ICPY_DOWN(1);
			downlongpress = 0;
		}
		if (KEY_ICPY_DOWN(0) > 100)
		{
			//按键长按超过100ms
			KEY_ICPY_DOWN(1);
			printf("KEYDOWN_PRES!\r\n");
			fflush(stdout);
		}

	}
	if (KEY_ICPY_DOWN(0) > 1000 && downlongpress == 0)
	{
		KEY_ICPY_DOWN(1);
		downlongpress = 1;
	}

	//---------------------------------------------左键
	//长按处理
	if (leftlongpress == 1)
	{
		//长按超过1000ms后置位，进入长按处理
		if (KEY_ICPY_LEFT(0) == 0)
		{
			//按键抬起
			KEY_ICPY_LEFT(1);
			leftlongpress = 0;
		}
		if (KEY_ICPY_LEFT(0) > 100)
		{
			//按键长按超过100ms
			KEY_ICPY_LEFT(1);
			printf("KEYLEFT_PRES!\r\n");
			fflush(stdout);
		}

	}
	if (KEY_ICPY_LEFT(0) > 1000 && leftlongpress == 0)
	{
		KEY_ICPY_LEFT(1);
		leftlongpress = 1;
	}
	//---------------------------------------------右键
	//长按处理
	if (rightlongpress == 1)
	{
		//长按超过1000ms后置位，进入长按处理
		if (KEY_ICPY_RIGHT(0) == 0)
		{
			//按键抬起
			KEY_ICPY_RIGHT(1);
			rightlongpress = 0;
		}
		if (KEY_ICPY_RIGHT(0) > 100)
		{
			//按键长按超过100ms
			KEY_ICPY_RIGHT(1);
			printf("KEYRIGHT_PRES!\r\n");
			fflush(stdout);
		}

	}
	if (KEY_ICPY_RIGHT(0) > 1000 && rightlongpress == 0)
	{
		KEY_ICPY_RIGHT(1);
		rightlongpress = 1;
	}
	//---------------------------------------------短按键
	key = KEY_Scan(0);	//得到键值
	if (key)
	{
		switch (key)
		{
		case KEYUP_PRES:
			printf("KEYUP_PRES!\r\n");
			break;
		case KEYDOWN_PRES:
			printf("KEYDOWN_PRES!\r\n");
			break;
		case KEYRIGHT_PRES:
			printf("KEYRIGHT_PRES!\r\n");
			break;
		case KEYLEFT_PRES:
			printf("KEYLEFT_PRES!\r\n");
			break;
		case KEYOK_PRES:
			printf("KEYOK_PRES!\r\n");
			break;
		case KEYM1_PRES:
			printf("KEYM1_PRES!\r\n");
			break;
		case KEYM2_PRES:
			printf("KEYM2_PRES!\r\n");
			break;
		case KEY_PWR_CAN_PRES:
			printf("KEY_PWR_CAN_PRES!\r\n");
			break;
		case KEY_ALL_PRES:
			printf("KEY_ALL_PRES!\r\n");
			break;
		}
	}
}
//充电扫描按键流程
void CHGKEYTASK(u8 en)
{
	u8 key = 0;
	if (KEY_POWER(0) > 3000 && en == 1)
	{
		printf("FROM_CHG_GO_INTO_MAIN!\r\n");
		fflush(stdout);
		KEY_POWER(1);
		while (MAINBATCHECKTASK(2) == 0) ;
		u16 batvol = MAINBATCHECKTASK(1);
		printf("CHG_PWRON_BAT_VOL %d!\r\n", batvol);
		fflush(stdout);
		if (batvol <= BATNOLOADTHR)
		{
			//显示电量太低提示
			ICPX_Diplay_Image(54, 89, ICOPYX_IMAGES[9]);
			for (u8 i = 0; i < 5; i++)
			{
				printf("CHG_PWRON_BAT_VOL_TOO_LOW!\r\n", batvol);
				fflush(stdout);
				g_Tim2Array[eTim3] = 0;
				while (IS_TIMEOUT_1MS(eTim3, 300))
				{
					ST7789_ShowString(30, 180, "BATTERY IS TOO LOW", WHITE, ICPX_BLUE_BAK, 20, 0);
				}
				g_Tim2Array[eTim3] = 0;
				while (IS_TIMEOUT_1MS(eTim3, 300))
				{
					ST7789_ShowString(30, 180, "                  ", WHITE, ICPX_BLUE_BAK, 20, 0);
				}
			}
			ICPX_Charge_Screen(1);
		}
		else
		{
			startmode = START_MODE_BAT;
			isstarting = 1;
			ICPX_Booting_Screen(1);//初始化开机界面状态
			ICPX_Charge_Screen(1);//初始化充电界面状态
		}
	}
}
////////////////////////////////////////////////////////////////////   按键相关END   /////////////////////

//OTG状态判断，主流程扫描运行
//在otg插入时需要断开充电开关，防止回流
//在otg拔出后启用充电开关
void CHARGE_OTG()
{
	//判断是否插入otg,按需控制充电开关
	if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_1))
	{
		turnoffchg();
	}
	else
	{
		turnonchg();
	}
}
//扫描充电状态流程
//输入为0的时候才判断（并且输出到标准输出）
//输入为1的时候不进行判断，而是直接返回充电状态
u8 MAINCHARGETASK(u8 what)
{
	static u8 chargestate = 0;
	if (what == 1)
	{
		return chargestate;
	}
	else
	{
		if (VCCvol > VCCTHRHIGH && chargestate == 0)
		{
			chargestate = 1;
			printf("CHARGING!\r\n");
		}
		else if (VCCvol <= VCCTHRLOW && chargestate == 1)
		{
			chargestate = 0;
			printf("DISCHARGIN!\r\n");
		}
	}

}
//电池检测流程
//输入为0的时候进行电量检测，包含低电量关机
//输入为1的时候不进行判断，而是直接返回电池电压状态（上个周期采集的）
//输入为2的时候强制开始新的电压判断（无视充电等待时间）
u16 MAINBATCHECKTASK(u8 what)
{
	static u16 batvolsense = 0;
	static u8 step = 0;
	//上一个采集流程
	//0 空闲状态（此时充电，需要等待一段时间）
	//等待一段时间
	//关闭充电过程
	//1 充电关闭（需要等待后采集）
	//等待一段时间
	//采集
	//下一个采集流程
	static u8 inprocess = 0;
	static u8 updateok = 0;

	if (what == 1)
	{
		return batvolsense;    //返回值
	}
	if (what == 0)
	{
		if (step == 0 && inprocess == 0)//充电，重置计时器
		{
			g_Tim2Array[eTimbat] = 0;
			inprocess = 1;//标志等待流程开始
		}
		else if (step == 0 && inprocess == 1 && g_Tim2Array[eTimbat] >= 10000) //充电等待流程结束
		{
			turnoffchg();
			step = 1;
			inprocess = 0;
			updateok = 0;
		}
		else if (step == 1 && inprocess == 0) //充电关闭了，等待采集开始
		{
			g_Tim2Array[eTimbat] = 0;
			inprocess = 1;//标志等待流程开始
		}

		else if (step == 1 && inprocess == 1 && g_Tim2Array[eTimbat] >= 1500) //等待电压回落
		{
			//采集
			batvolsense = ICPX_BAT_VOL_GATHER(1);
			updateok = 1;
			turnonchg();
			//归位，回到初始流程
			step = 0;
			inprocess = 0;
			//printf("batvol 0 %d \r\n", batvolsense);
			//fflush(stdout);
		}
	}
	if (what == 2)
	{
		if (batvolsense != 0)
		{
			return 1;
		}
		if (step == 0)//充电，重置计时器
		{
			inprocess = 1;//标志等待流程开始
			turnoffchg();
			step = 1;
			inprocess = 0;
			updateok = 0;
		}
		else if (step == 1 && inprocess == 0) //充电关闭了，等待采集开始
		{
			g_Tim2Array[eTimbat] = 0;
			inprocess = 1;//标志等待流程开始
		}

		else if (step == 1 && inprocess == 1 && g_Tim2Array[eTimbat] >= 1500) //等待电压回落
		{
			//采集
			batvolsense = ICPX_BAT_VOL_GATHER(1);
			updateok = 1;
			turnonchg();
			//归位，回到初始流程
			step = 0;
			inprocess = 0;
			//printf("batvol 2 %d \r\n", batvolsense);
			//fflush(stdout);
		}
		return updateok;
	}
	//低电量关机判断
	if (batvolsense <= BATWITHLOADTHR && step == 0 && inprocess == 0)
	{
		g_Tim2Array[eTimbat] = 0;
		while (IS_TIMEOUT_1MS(eTimbat, 5));
		if (BATvolavl <= BATWITHLOADTHR)
		{
			printf("LOWBATTERY!!\r\n");
			ICPX_Booting_Screen(1);//初始化开机界面状态
			ICPX_Charge_Screen(1);//初始化充电界面状态
			SHUTDOWNMETH();
		}
	}

}
//电池电压到百分比计算
u32 BATVOL2PERCENT(u16 VOL)
{
	//100%	4.20V	1
	//90 %	4.06V		80%-100%	白
	//80 %	3.98V	1
	//70 %	3.92V		60%-80%		白
	//60 %	3.87V	1
	//50 %	3.82V		40%-60%		白
	//40 %	3.79V	1
	//30 %	3.77V		20%-40%		白
	//20 %	3.74V	1
	//10 %	3.68V		5%-20%		红
	//5 %	3.45V	1				关机
	//0 %	3.00V
#define P100VOL	4200
#define P80VOL	3980
#define P60VOL	3870
#define P40VOL	3790
#define P20VOL	3740
#define P5VOL	3450

	if (VOL > P80VOL)
	{
		//80-100
		return map(VOL, P80VOL, P100VOL, 80, 100);
	}
	else if (VOL > P60VOL)
	{
		//60-80
		return map(VOL, P60VOL, P80VOL, 60, 80);
	}
	else if (VOL > P40VOL)
	{
		//40-60
		return map(VOL, P40VOL, P60VOL, 40, 60);
	}
	else if (VOL > P20VOL)
	{
		//20-60
		return map(VOL, P20VOL, P40VOL, 20, 40);
	}
	else if (VOL > P5VOL)
	{
		//5-20
		return map(VOL, P5VOL, P20VOL, 5, 20);
	}
	else
	{
		//<5
		return 1;
	}
}
//电池采集流程
//输入1时启用判断下降沿
//输入0时直接采集
//输入2时采集内部基准（直接采集）
u16 ICPX_BAT_VOL_GATHER(u8 MODE)
{
	if (MODE == 1)
	{
		//需要停止充电之后再调用
		u16 LASTVOL = BATvolavl;
		u16 THISVOL = BATvolavl;
		while (  THISVOL < LASTVOL)
		{
			LASTVOL = THISVOL;
			THISVOL = BATvolavl;
		}
		return THISVOL;
	}
	else if (MODE == 0)
	{
		//此时直接采集
		return BATvolavl;
	}
	else if (MODE == 2)
	{
		//此时直接采集
		return Intvolavl;
	}
	else
	{
		//mode是错误值，返回0
		return 0;
	}
}
//电池动画函数，用来修正目标值防止跳变
u16 ICPX_BAT_VOL_REVICE(u8 what)
{
	//动画用于在电压跳变时修正电量，采用电压逐次追踪方法
	//变化时间不小于5分钟，以电压值差值运算
	//5分钟300000ms，跟踪值一次变化1，每次运算后得到下一个增长周期
	static u16 outvol = 0;
	static u16 aimvol = 0;
	static u32 timetowait = 0;
	static u8 mode = 0; // 模式0代表等待未开始(数据准备），模式1代表等待中

	if (what == 1)
	{
		return outvol;
	}
	if (outvol == 0)
	{
		outvol = MAINBATCHECKTASK(1);
	}

	aimvol = MAINBATCHECKTASK(1);

	if (aimvol != 0 && outvol != 0)
	{
		//两个值都有效，开始处理
		signed int dvalue = aimvol - outvol;	//目标值和当前值的差
		if (dvalue <= 1 && dvalue >= -1)
		{
			//差值太小
			timetowait = 10000;			//十秒后更新
			mode = 1;
		}
		else
		{
			if (dvalue > 0)
			{
				timetowait = 300000 / dvalue;    //更新一次需要的时间
			}
			if (dvalue < 0)
			{
				timetowait = 300000 / (-dvalue);    //更新一次需要的时间
			}
			mode = 1;
		}
		if (mode == 0)
		{
			g_Tim2Array[eTimbatmv] = 0;

		}
		else
		{
			if (g_Tim2Array[eTimbatmv] > timetowait)
			{
				g_Tim2Array[eTimbatmv] = 0;
				mode = 0;
				if (dvalue > 0)
				{
					outvol++;
				}
				if (dvalue < 0)
				{
					outvol--;
				}
				if (dvalue == 0)
				{
					return 0;
				}
				//printf("outvol %d aimvol %d\r\n", outvol, aimvol);
				//fflush(stdout);
			}
		}

	}



}

void STARTMODETASK(void)
{
	//注意，这个流程只会在唤醒时运行一次
	if (VCCvol > VCCTHRHIGH) //存在电源输入
	{
		//vcc开机
		startmode = START_MODE_VCC;
		printf("The power is already plugged in!\r\n");
		fflush(stdout);
	}
	else
	{
		//电池开机
		while (KEY_POWER(0) < 3000)
		{
			if (KEY_POWER(0) == 0)
			{
				printf("The press time is not long enough!\r\n");
				fflush(stdout);
				//立即睡眠
				ICPX_Standby();
			}
		}
		//到这里按键时间超过开机标准了，开始处理开机
		while (MAINBATCHECKTASK(2) == 0);
		u16 batvol = MAINBATCHECKTASK(1);
		if (batvol <= BATNOLOADTHR)
		{
			printf("lowbattery,vol:%d\r\n", batvol);
			fflush(stdout);
			//立即睡眠
			ICPX_Standby();
		}
		printf("all done! power on!\r\n");
		fflush(stdout);
		KEY_POWER(1);
		startmode = START_MODE_BAT;
		ICPX_Booting_Screen(1);//初始化开机界面状态
		ICPX_Charge_Screen(1);//初始化充电界面状态
	}

}
void SHUTDOWNMETH()
{
	printf("SHUTDOWN H3!\r\n");
	fflush(stdout);
	u8 TIMES = 0;
	while (1)
	{
		printf("ARE YOU OK?\r\n");
		fflush(stdout);//询问目标存活状态
		g_Tim2Array[eTim3] = 0;
		while (IS_TIMEOUT_1MS(eTim3, 500))
		{
			CLI_RUN();
		}
		if (hasbak != 0) //有回复
		{
			TIMES = 0;
			hasbak = 0;
		}
		else
		{
			TIMES++;
		}
		if (TIMES > 5)
		{
			break;
		}
	}
	printf("OK! You are died\r\n");
	printf("Prepare to SHUTDOWN!\r\n");
	fflush(stdout);
	//延时一个关机时长
	g_Tim2Array[eTim1] = 0;
	while (IS_TIMEOUT_1MS(eTim1, 10000))
	{
		ICPX_Shutdown_Screen(0);
	}
	printf("Bye!\r\n");
	fflush(stdout);
	SPISELST();
	turnoffh3();
	ICPX_Shutdown_Screen(1);
	if (VCCvol > VCCTHRHIGH)
	{
		//充电已插入，不可以休眠
		startmode = START_MODE_VCC;
		ICPX_Charge_Screen(1);//初始化第一次显示的变量，防止不刷新全屏
	}
	else
	{
		ICPX_Standby();
	}
}
u32 GetMCUID(void)
{
	return (*(u32*)(0x1FFFF7E8));
}
void ICPX_Standby()
{
	printf("i'm sleepy!\r\n");
	fflush(stdout);
	GPIO_ResetBits(FLASH_PWR_GPIO_Port, FLASH_PWR_Pin);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//开电源管理时钟PWR_Regulator_LowPower

	PWR_WakeUpPinCmd(ENABLE);//使能唤醒引脚，默认PA0

	PWR_EnterSTANDBYMode();//进入待机
	//PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI|PWR_STOPEntry_WFE);//进入停机
}




void setback()
{
	hasbak = 1;
}
u32 map(u32 x, u32 in_min, u32 in_max, u32 out_min, u32 out_max)
{
	return (u32)((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

