#include "ICOPYX_BSP.h"

u8 startmode = START_MODE_NONE;
u8 isstarting = 1;

//板载gpio初始化
void ICPX_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能A端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能B端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能C端口时钟
	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
	
	GPIO_InitStructure.GPIO_Pin = FLASH_PWR_Pin;
	GPIO_Init(FLASH_PWR_GPIO_Port, &GPIO_InitStructure);
	GPIO_SetBits(FLASH_PWR_GPIO_Port, FLASH_PWR_Pin);
	
	GPIO_InitStructure.GPIO_Pin = H3_PWR_ON_OFF_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(H3_PWR_ON_OFF_GPIO_Port, &GPIO_InitStructure);
	GPIO_ResetBits(H3_PWR_ON_OFF_GPIO_Port, H3_PWR_ON_OFF_Pin);
	
	GPIO_InitStructure.GPIO_Pin = PM_PWR_ON_OFF_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(PM_PWR_ON_OFF_GPIO_Port, &GPIO_InitStructure);
	GPIO_ResetBits(PM_PWR_ON_OFF_GPIO_Port, PM_PWR_ON_OFF_Pin);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_InitStructure.GPIO_Pin = PM_BUTTON_Pin;
	GPIO_Init(PM_BUTTON_GPIO_Port, &GPIO_InitStructure);
	GPIO_ResetBits(PM_BUTTON_GPIO_Port, PM_BUTTON_Pin);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //输入
	
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
	memset(datatemp, 0, sizeof(datatemp));
	memset(dataprint, 0, sizeof(dataprint));
	printf("W25Qxx ID:%02X\r\n", W25QXX_TYPE);
	W25QXX_ReadUID(datatemp);
	sprintf(dataprint, "0x%02X%02X%02X%02X%02X%02X%02X%02X", datatemp[0], datatemp[1], datatemp[2], datatemp[3], datatemp[4], datatemp[5], datatemp[6], datatemp[7]);
	printf("W25Qxx UID:%s\r\n", dataprint);
	//ST7789_ShowString(8, 164, dataprint, WHITE, BLACK, 32, 1);
	memset(datatemp, 0, sizeof(datatemp));
	memset(dataprint, 0, sizeof(dataprint));
	W25QXX_Read(datatemp, 0x000B5080, (u32)8);
	sprintf(dataprint, "0x%02X%02X%02X%02X%02X%02X%02X%02X", datatemp[0], datatemp[1], datatemp[2], datatemp[3], datatemp[4], datatemp[5], datatemp[6], datatemp[7]);
	printf("readdata:%s\r\n", dataprint);
	//ST7789_ShowString(8, 196, dataprint, WHITE, BLACK, 32, 1);
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
		for (i = 0;i < 0x00400000;i += 0x10000)
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
	
	for (i = 0;i < blocks;i++)//刷整块
	{
		W25QXX_FastRead(LineData, curaddr, inputimage.XSize * 2 * blocklines);
		curaddr += inputimage.XSize * 2 * blocklines;
		ST7789_ShowPicture(x, y + Y, inputimage.XSize, blocklines, LineData);
		
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
void ICPX_Select_Font_Data_all(u8 *datas, u16 num, u16 numtoread, typ_fontlib_s fontlab)
{
	u32 aimbaseaddr = fontlab.StartADDR + num * fontlab.DataSize;
	W25QXX_FastRead(datas, aimbaseaddr, numtoread);
	return ;
}

u32 GetMCUID(void)
{
	return (*(u32*)(0x1FFFF7E8));
}

void ICPX_Standby()
{
	GPIO_ResetBits(FLASH_PWR_GPIO_Port, FLASH_PWR_Pin);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//开电源管理时钟PWR_Regulator_LowPower

	PWR_WakeUpPinCmd(ENABLE);//使能唤醒引脚，默认PA0

	PWR_EnterSTANDBYMode();//进入待机
	//PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI|PWR_STOPEntry_WFE);//进入停机
}

void ICPX_Charge_Screen(u8 init)
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
		while (IS_TIMEOUT_1MS(eTim2, 500))
		{
			ICPX_Diplay_Image(0, 0, ICOPYX_IMAGES[0]);
			ST7789_BL_ON();
		}
		firstshow = 1;
	}
	else
	{
		g_Tim2Array[eTim2] = 0;
		while (IS_TIMEOUT_1MS(eTim2, 500))
		{
			ICPX_Diplay_Image(54, 89, ICOPYX_IMAGES[9]);
		}
		
	}
	g_Tim2Array[eTim2] = 0;
	while (IS_TIMEOUT_1MS(eTim2, 500))
	{
		ICPX_Diplay_Image(54, 89, ICOPYX_IMAGES[1]);
	}
	g_Tim2Array[eTim2] = 0;
	while (IS_TIMEOUT_1MS(eTim2, 500))
	{
		ICPX_Diplay_Image(68, 89, ICOPYX_IMAGES[2]);
	}
	g_Tim2Array[eTim2] = 0;
	while (IS_TIMEOUT_1MS(eTim2, 500))
	{
		ICPX_Diplay_Image(82, 89, ICOPYX_IMAGES[3]);
	}
	g_Tim2Array[eTim2] = 0;
	while (IS_TIMEOUT_1MS(eTim2, 500))
	{
		ICPX_Diplay_Image(96, 89, ICOPYX_IMAGES[4]);
	}
	g_Tim2Array[eTim2] = 0;
	while (IS_TIMEOUT_1MS(eTim2, 500))
	{
		ICPX_Diplay_Image(119, 89, ICOPYX_IMAGES[5]);
	}
	g_Tim2Array[eTim2] = 0;
	while (IS_TIMEOUT_1MS(eTim2, 500))
	{
		ICPX_Diplay_Image(133, 89, ICOPYX_IMAGES[6]);
	}
	g_Tim2Array[eTim2] = 0;
	while (IS_TIMEOUT_1MS(eTim2, 500))
	{
		ICPX_Diplay_Image(147, 89, ICOPYX_IMAGES[7]);
	}
	g_Tim2Array[eTim2] = 0;
	while (IS_TIMEOUT_1MS(eTim2, 500))
	{
		ICPX_Diplay_Image(161, 89, ICOPYX_IMAGES[8]);
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
			ICPX_Diplay_Image(0, 0, ICOPYX_IMAGES[10]);
		}
		firstshow = 1;
	}
	ST7789_ShowString(60, 200, "Booting...", WHITE, BLACK, 20, 1);
	g_Tim2Array[eTim2] = 0;
	while (IS_TIMEOUT_1MS(eTim2, 500))
	{
	//	ICPX_DNA_CIRCLE();
	}
}

void ICPX_Booting_Error_Screen(void)
{
	g_Tim2Array[eTim2] = 0;
	while (IS_TIMEOUT_1MS(eTim2, 1))
	{
		ST7789_ShowString(0, 0, "boot error!!", YELLOW, BLACK, 32, 0);
	}
}


double tsin(double x)
{
	double g = 0; //g为sinx()最终结果 
	double t = x; //初始值x赋给t 
	int n = 1; //n 用于求分母 
	do {
		g += t;
		n++;
		t = -t * x * x / (2 * n - 1) / (2 * n - 2);
	}while (fabs(t) >= 1e-6) ; //fabs()求double型变量的绝对值 
	return g;
}

double tcos(double x)
{
	x += PII; 

	if (x > PII) 
		x -= 2 * PII; 

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
	if (needbl)
	{
		for (node = 0; node < 360;node++)
		{
			sinb[node] = ((tsin(x * PII / 180) * 1000) + 1000) * y_frame / (2000);
			sizesinb[node] = tsin((x - 90) % 360* PII / 180) * 2 * node_change * node_radius;
			cosb[node] = ((tcos(x * PII / 180) * 1000) + 1000) * y_frame / (2000);
			sizecosb[node] = tcos((x - 90) % 360 * PII / 180) * 2 * node_change * node_radius;
			x += xstep;
		}
		//表构建完成
		needbl = 0;
	}
	//表准备结束，上面代码不会再运行了

	
	static u16 aa = (max_node_number - 1);
	static u16 bb;
	bb = dnaangle / aa;
	static u16 cc;
	cc = frame * dnaspeed / aa;
	//开始查表运算目标点位置
	for(node = 0 ; node < max_node_number ; node++)
	{
		static u8 zerotime = 0;
		//循环每个节点
		int aimangle = (node * bb + cc) % 360;
		nodes[node][0] = sinb[aimangle];
		nodes[node][1] = sizesinb[aimangle];
		nodes[node][2] = cosb[aimangle];
		nodes[node][3] = sizecosb[aimangle];
		//过零检测
		if(aimangle == 0)
		{
			zerotime++;
		}
		//检测一周期完成
		if(zerotime == 3)
		{
			frame = 0;
			zerotime = 0;
		}
	}
	//下一帧
	frame++;
	static int fstnodex = dnaxstart + (node_radius + (node_change*node_radius)) / 2;
	static int nodestep = (dnaxstop - dnaxstart - (node_radius + (node_change*node_radius))) / (max_node_number - 1);
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

//主扫描按键流程
void MAINKEYTASK(void)
{
	u8 key = 0;
	if (KEY_POWER(0) > 3000)
	{
		KEY_POWER(1);
		if (isstarting != 1)
		{
			SHUTDOWNMETH(1000);//关机指令
		}
	}
	key = KEY_Scan(0);	//得到键值
	if(key)
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
void CHGKEYTASK(void)
{
	u8 key = 0;
	if (KEY_POWER(0) > 3000)
	{
		//printf("FROM_CHG_GO_INTO_MAIN!\r\n");
		//fflush(stdout);
		KEY_POWER(1);
		if (BATvol <= BATNOLOADTHR)
		{
			//显示电量太低提示
			//添加完下面三行删除
			startmode = START_MODE_BAT;
			ICPX_Booting_Screen(1);//初始化开机界面状态
			ICPX_Charge_Screen(1);//初始化充电界面状态
		}
		else
		{
			startmode = START_MODE_BAT;
			ICPX_Booting_Screen(1);//初始化开机界面状态
			ICPX_Charge_Screen(1);//初始化充电界面状态
		}
	}
}
//扫描充电状态流程
void MAINCHARGETASK(void)
{	
}
void MAINBATCHECKTASK(void)
{
	if (BATvol <= BATWITHLOADTHR)
	{
		printf("lowbattery");
		ICPX_Booting_Screen(1);//初始化开机界面状态
		ICPX_Charge_Screen(1);//初始化充电界面状态
		SHUTDOWNMETH(1000);
	}
	
}
//开机模式判断
void STARTMODETASK(void)
{
	//注意，这个流程只会在唤醒时运行一次
	if(VCCvol > VCCTHR)//存在电源输入
	{
		//vcc开机
		startmode = START_MODE_VCC;
	}
	else
	{
		//电池开机
		while(KEY_POWER(0) < 3000)
		{
			if (BATvol <= BATNOLOADTHR)
			{
				//立即睡眠
				ICPX_Standby();
			}
			if (KEY_POWER(0) == 0)
			{
				//立即睡眠
				ICPX_Standby();
			}
		}
		KEY_POWER(1);
		startmode = START_MODE_BAT;
		ICPX_Booting_Screen(1);//初始化开机界面状态
		ICPX_Charge_Screen(1);//初始化充电界面状态
	}
	
}
void SHUTDOWNMETH(u16 delay)
{
	printf("SHUTDOWN H3!\r\n");
	//fflush(stdout);
	//延时一个关机时长
	g_Tim2Array[eTim1] = 0;
	while (IS_TIMEOUT_1MS(eTim1, delay))
	{
	}
	GPIO_ResetBits(H3_PWR_ON_OFF_GPIO_Port, H3_PWR_ON_OFF_Pin);
	if (VCCvol > VCCTHR)
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
