#include "sys_command_line.h"
#include "usart.h"
#include "ICOPYX_BSP.h"
//#define commdebug
static uint8_t cli_reboot(void *para, uint8_t len);
static uint8_t cli_bl(void *para, uint8_t len);
static uint8_t cli_fillscreen(void *para, uint8_t len);
static uint8_t cli_fillsquare(void *para, uint8_t len);
static uint8_t cli_showsimbol(void *para, uint8_t len);
static uint8_t cli_showstring(void *para, uint8_t len);
static uint8_t cli_showpicture(void *para, uint8_t len);
static uint8_t cli_presspm3(void *para, uint8_t len);
static uint8_t cli_ledpm3(void *para, uint8_t len);
static uint8_t cli_turnoffpm3(void *para, uint8_t len);
static uint8_t cli_turnonpm3(void *para, uint8_t len);
static uint8_t cli_restartpm3(void *para, uint8_t len);
static uint8_t cli_volbat(void *para, uint8_t len);
static uint8_t cli_pctbat(void *para, uint8_t len);
static uint8_t cli_volinput(void *para, uint8_t len);
static uint8_t cli_charge(void *para, uint8_t len);
static uint8_t cli_h3start(void *para, uint8_t len);
static uint8_t cli_multicmd(void *para, uint8_t len);
static uint8_t cli_setbaklight(void *para, uint8_t len);
static uint8_t cli_targetalive(void *para, uint8_t len);
static uint8_t cli_getversion(void *para, uint8_t len);
static uint8_t cli_getid(void *para, uint8_t len);
static uint8_t cli_lcd2h3(void *para, uint8_t len);
static uint8_t cli_lcd2st(void *para, uint8_t len);
static uint8_t cli_shutdowning(void *para, uint8_t len);
static uint8_t cli_planshutdown(void *para, uint8_t len);
typedef struct {
#define HANDLE_LEN 1024
	u8 buff[HANDLE_LEN];
	u16 len;
} HANDLE_TYPE_S;

pFunction Jump_To_Application;
uint32_t JumpAddress;

RX_BUFF_TYPE cli_rx_buff; /* 128byte FIFO, saving commands from the terminal */

//const char CLI_Cmd_Reboot[] =
//    "[reboot]\r\n"
//    " * reboot MCU\r\n"
//    "\r\n";


//命令序列
const COMMAND_S CLI_Cmd[] = {
	//系统指令
	/* cmd              cmd help            init func.      func. */
	//{ "reboot", NULL/*CLI_Cmd_Reboot*/, NULL, cli_reboot },
	{ "gotobl", NULL, NULL, cli_bl },
	//pm3状态操作指令序列
	{ "ledpm3", NULL, NULL, cli_ledpm3},
	{ "presspm3", NULL, NULL, cli_presspm3},
	//pm3电源指令序列
	{ "turnonpm3", NULL, NULL, cli_turnonpm3},
	{ "turnoffpm3", NULL, NULL, cli_turnoffpm3},
	{ "restartpm3", NULL, NULL, cli_restartpm3},
	//电源电压指令序列
	{ "volbat", NULL, NULL, cli_volbat},
	{ "pctbat", NULL, NULL, cli_pctbat},
	{ "volvcc", NULL, NULL, cli_volinput},
	//查询充电状态指令序列(耗时问题)
	{ "charge", NULL, NULL, cli_charge},
	{ "h3start", NULL, NULL, cli_h3start},
	//转交屏幕权限指令
	{ "givemelcd", NULL, NULL, cli_lcd2h3},
	{ "giveyoulcd", NULL, NULL, cli_lcd2st},
	{ "shutdowning", NULL, NULL, cli_shutdowning},
	
	//显示操作指令
	{ "fillscreen", NULL, NULL, cli_fillscreen },
	{ "fillsquare", NULL, NULL, cli_fillsquare },
	{ "showsimbol", NULL, NULL, cli_showsimbol },
	{ "showstring", NULL, NULL, cli_showstring },
	{ "showpicture", NULL, NULL, cli_showpicture},
	{ "setbaklight", NULL, NULL, cli_setbaklight},
	
	//多重指令同步传输
	{ "multicmd", NULL, NULL, cli_multicmd },
	
	//关机命令
	{ "plan2shutdown", NULL, NULL, cli_planshutdown },
	
	//询问存活的回复
	{ "i'm alive", NULL, NULL, cli_targetalive },
	{ "version", NULL, NULL, cli_getversion },
	{ "idid", NULL, NULL, cli_getid },
};

//重启实现
static uint8_t cli_reboot(void *para, uint8_t len)
{
	PRINTF("\r\n[END]: System Rebooting...");
	SYSTEM_REBOOT();
	return TRUE;
}
//重启到bl实现
static uint8_t cli_bl(void *para, uint8_t len)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	//Usart1 NVIC 配置
	g_Tim2Array[eTim1] = 0;
	while (IS_TIMEOUT_1MS(eTim1, 500))
	{
	}
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;//IRQ通道关闭
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	if (((*(__IO uint32_t*)BLAddress) & 0x2FFE0000) == 0x20000000)
	{
		PRINTF("\r\n[END]: Execute Bl Program \r\n");
		PRINTF("------------------------------------------\r\n");
		BspTim2Close();
		//跳转至bl代码
		JumpAddress = *(__IO uint32_t*)(BLAddress + 4);
		Jump_To_Application = (pFunction) JumpAddress;
		//初始化bl程序的堆栈指针
		__set_MSP(*(__IO uint32_t*) BLAddress);
		Jump_To_Application();
	}
	else
	{
		PRINTF("\r\nno Bl Program\r\n");
	}
	return TRUE;
}
//读取pm3led实现
static uint8_t cli_ledpm3(void *para, uint8_t len)
{
	if (GPIO_ReadInputDataBit(PM_4LED_GPIO_Port, PM_4LED_Pin))
	{
		printf("#LED:1");
		fflush(stdout);
	}
	else
	{
		printf("#LED:0");
		fflush(stdout);
	}
	return TRUE;
}
//打开pm3实现
static uint8_t cli_turnonpm3(void *para, uint8_t len)
{
	turnonpm3();
	return TRUE;
}
//关掉pm3实现
static uint8_t cli_turnoffpm3(void *para, uint8_t len)
{
	turnoffpm3();
	return TRUE;
}
//重启pm3实现
static uint8_t cli_restartpm3(void *para, uint8_t len)
{
	g_Tim2Array[eTim3] = 0;
	while (IS_TIMEOUT_1MS(eTim3, 30))//关闭pm3延时
	{
		turnoffpm3();
	}
	turnonpm3();
	return TRUE;
}
//按下pm3按钮实现
static uint8_t cli_presspm3(void *para, uint8_t len)
{
	//pm3按钮拉高有效，拉高300ms后拉低
	g_Tim2Array[eTim3] = 0;
	while(IS_TIMEOUT_1MS(eTim3, 300))
	{
		GPIO_SetBits(PM_BUTTON_GPIO_Port, PM_BUTTON_Pin);
	}
	GPIO_ResetBits(PM_BUTTON_GPIO_Port, PM_BUTTON_Pin);
	return TRUE;
}
//读取电池电压实现
static uint8_t cli_volbat(void *para, uint8_t len)
{	
	printf("#batvol:%d",1234);//BATvolavl);
	fflush(stdout);
	return TRUE;
}
//读取电池百分比实现
static uint8_t cli_pctbat(void *para, uint8_t len)
{	
	printf("#batpct:%d", BATVOL2PERCENT(ICPX_BAT_VOL_REVICE(1)));
	fflush(stdout);
	return TRUE;
}

//读取电源电压实现
static uint8_t cli_volinput(void *para, uint8_t len)
{
	printf("#vccvol:%d",1234);//VCCvolavl);
	fflush(stdout);
	return TRUE;
}
//查询充电是否完成实现
static uint8_t cli_charge(void *para, uint8_t len)
{
	//TODO: 查询输入状态
	printf("#charge:%d", MAINCHARGETASK(1));
	fflush(stdout);
	return TRUE;
}
//开机完成指令
static uint8_t cli_h3start(void *para, uint8_t len)
{
	//isstarting = 0;
	return TRUE;
}	
//lcd转交给h3指令
static uint8_t cli_lcd2h3(void *para, uint8_t len)
{
	isstarting = 0;
	SPISELH3();
	return TRUE;
}

//lcd转交给st指令
static uint8_t cli_lcd2st(void *para, uint8_t len)
{
	SPISELST();
	return TRUE;
}

//h3关机指令
static uint8_t cli_shutdowning(void *para, uint8_t len)
{
	ICPX_Shutdown_Screen(0);
	return TRUE;
}


//填充屏幕实现
//指令        参数颜色   参数颜色值      
//fillscreen  C(0x43)  (0x00)  (0x00)  
//配置帧结束   
//A(0x41)
//66 69 6C 6C 73 63 72 65 65 6E 43 ff ff 41 0D 0A
static uint8_t cli_fillscreen(void *para, uint8_t len)
{
	uint8_t *pTemp;
	pTemp = (uint8_t *)para;
	
	u8 updating = 1;
	
	u16 color = 0;
	
	
	if ((0 < len) && (NULL != pTemp)) {
		while (updating && '\r' != *pTemp)
		{
			if ('C' == *pTemp) {
				//颜色帧传输
				pTemp++;
				color += (*pTemp) << 8;
				pTemp++;
				color += (*pTemp);
			}
			else if ('A' == *pTemp) {
				//帧传输结束，可以显示
				updating = 0;
#ifdef commdebug
				PRINTF("FillScreen:color: %04X \r\n ",
					\
					color);
#endif
				ST7789_Fill(0, 0, ST7789_W, ST7789_H, color);
			}
			else {
				//错误数据帧
				return FALSE;
			}
			pTemp++;
		}
	}
	return TRUE;
}
//填充方形区域实现
//指令        参数颜色   参数颜色值      参数座标头  参数起始座标值x   参数起始座标值y  参数结束座标值x   参数结束座标值y 
//fillsquare  C(0x43)  (0x00)  (0x00)  P(0x50)   (0x00)(0x00)     (0x00)(0x00)	 (0x00)(0x00)     (0x00)(0x00)
//配置帧结束   
//A(0x41)
//66 69 6C 6C 73 71 75 61 72 65 43 00 ff 50 00 00 00 00 00 1a 00 1a 41 0D 0A
static uint8_t cli_fillsquare(void *para, uint8_t len)
{
	uint8_t *pTemp;
	pTemp = (uint8_t *)para;
	
	u8 updating = 1;
	
	u16 color = 0;
	u16 startx = 0;
	u16 starty = 0;
	u16 stopx = 0;
	u16 stopy = 0;
	
	
	if ((0 < len) && (NULL != pTemp)) {
		while (updating && '\r' != *pTemp)
		{
			if ('C' == *pTemp) {
				//颜色帧传输
				pTemp++;
				color += (*pTemp) << 8;
				pTemp++;
				color += (*pTemp);
			}
			else if ('P' == *pTemp) {
				//座标传输
				pTemp++;
				startx += (*pTemp) << 8;
				pTemp++;
				startx += (*pTemp);
				pTemp++;
				starty += (*pTemp) << 8;
				pTemp++;
				starty += (*pTemp);
				pTemp++;
				stopx += (*pTemp) << 8;
				pTemp++;
				stopx += (*pTemp);
				pTemp++;
				stopy += (*pTemp) << 8;
				pTemp++;
				stopy += (*pTemp);
			}
			else if ('A' == *pTemp) {
				//帧传输结束，可以显示
				updating = 0;
#ifdef commdebug
				PRINTF("FillSquare: position:(%d,%d) stop:(%d,%d) color: %04X \r\n ",
					\
					startx,
					starty,
					stopx,
					stopy,
					color);
#endif
				ST7789_Fill(startx, starty, stopx, stopy, color);
			}
			else {
				//错误数据帧
				return FALSE;
			}
			pTemp++;
		}
	}
	return TRUE;
}
//显示符号实现
//指令        参数座标头  参数座标值x   参数座标值y    参数尺寸头   参数尺寸值                  
//showsimbol  P(0x50)   (0x00)(0x00) (0x00)(0x00)  S(0X53)     (0x00)(0x00)  
//参数颜色头   参数前景颜色值  参数背景颜色值  参数符号ID 参数符号ID值
//C(0x43)     (0x00)(0x00)  (0x00)(0x00)   I(0x49)   (0x00)
//配置帧结束   
//A(0x41)
//73 68 6F 77 73 69 6D 62 6F 6C 50 00 0A 00 0A 43 ff 00 00 08 49 01 53 0A 12 41 0D 0A
static uint8_t cli_showsimbol(void *para, uint8_t len)
{
	uint8_t *pTemp;
	pTemp = (uint8_t *)para;
	
	u8 updating = 1;
	
	u16 colorf = 0;
	u16 colorb = 0;
	u16 x = 0;
	u16 y = 0;
	u8 sizex = 0;
	u8 sizey = 0;
	u8 num = 0;
	
	if ((0 < len) && (NULL != pTemp)) {
		while (updating && '\r' != *pTemp)
		{
			if ('C' == *pTemp) {
				//颜色帧传输
				pTemp++;
				colorf += (*pTemp) << 8;
				pTemp++;
				colorf += (*pTemp);
				pTemp++;
				colorb += (*pTemp) << 8;
				pTemp++;
				colorb += (*pTemp);
			}
			else if ('P' == *pTemp) {
				//座标传输
				pTemp++;
				x += (*pTemp) << 8;
				pTemp++;
				x += (*pTemp);
				pTemp++;
				y += (*pTemp) << 8;
				pTemp++;
				y += (*pTemp);
			}
			else if ('S' == *pTemp) {
				//尺寸帧传输
				pTemp++;
				sizex += (*pTemp);
				pTemp++;
				sizey += (*pTemp);
			}
			else if ('I' == *pTemp) {
				//id传输
				pTemp++;
				num += (*pTemp);
			}
			else if ('A' == *pTemp) {
				//帧传输结束，可以显示
				updating = 0;
#ifdef commdebug
				PRINTF("ShowSimbol:%d position:(%d,%d) size:(%d,%d) fcolor: %04X bcolor: %04X\r\n ",
					\
					num,
					x,
					y,
					sizex,
					sizey,
					colorf,
					colorb);
#endif
				ST7789_ShowSimbol(x, y, num, colorf, colorb, sizex, sizey, 1);//显示一个符号
			}
			else {
				//错误数据帧
				return FALSE;
			}
			pTemp++;
		}
	}
	return TRUE;
}
//显示字符串实现
//指令        参数座标头  参数座标值x   参数座标值y    参数尺寸头   参数尺寸值(x,y),其中x无用                  
//showstring  P(0x50)   (0x00)(0x00) (0x00)(0x00)  S(0X53)     (0x00)(0x00)  
//参数颜色头   参数前景颜色值  参数背景颜色值  参数字符串头  参数字符串内容
//C(0x43)     (0x00)(0x00)  (0x00)(0x00)   I(0x49)      Nx(0x00)
//配置帧结束   
//A(0x41)
//字符串内容：
//起始符号  长度(不包含头但是包含结尾符的总字节数）  	数据               结尾符号
//(0x02)   (0x00)                               (0x00)(0x00)....  (0x03)
//73 68 6F 77 73 74 72 69 6E 67 50 00 0A 00 0A 43 ff 00 00 08 49 02 03 31 32 03 53 0A 20 41 0D 0A
static uint8_t cli_showstring(void *para, uint8_t len)
{
	uint8_t *pTemp;
	pTemp = (uint8_t *)para;
	u16 color = 0;
		
	u8 updating = 1;
	u8 stringlength = 0;
	u8 stringin[64];
	u16 colorf = 0;
	u16 colorb = 0;
	u16 x = 0;
	u16 y = 0;
	u8 sizex = 0;
	u8 sizey = 0;
	u8 num = 0;

	
	
	if ((0 < len) && (NULL != pTemp)) {
		while (updating && '\r' != *pTemp)
		{
			if ('C' == *pTemp) {
				//颜色帧传输
				pTemp++;
				colorf += (*pTemp) << 8;
				pTemp++;
				colorf += (*pTemp);
				pTemp++;
				colorb += (*pTemp) << 8;
				pTemp++;
				colorb += (*pTemp);
			}
			else if ('P' == *pTemp) {
				//座标传输
				pTemp++;
				x += (*pTemp) << 8;
				pTemp++;
				x += (*pTemp);
				pTemp++;
				y += (*pTemp) << 8;
				pTemp++;
				y += (*pTemp);
			}
			else if ('S' == *pTemp) {
				//尺寸帧传输
				pTemp++;
				sizex += (*pTemp);
				pTemp++;
				sizey += (*pTemp);
			}
			else if ('I' == *pTemp) {
				//字符串帧传输（0x02+长度+数据+0x03）
				pTemp++;
				if (*pTemp != 0x02)
				{
					//错误数据帧
					return FALSE;
				}
				pTemp++;
				stringlength = *pTemp;
				if (stringlength <= 0)
				{
					return TRUE;
				}
				memset(stringin, '\0', 64);
				for (int i = 0; i < stringlength - 1; i++)
				{
					pTemp++;
					stringin[i] = *pTemp;
					
				}
				pTemp++;
				if (*pTemp != 0x03)
				{
					//错误数据帧
					return FALSE;
				}
			}
			else if ('A' == *pTemp) {
				//帧传输结束
				updating = 0;
#ifdef commdebug
				PRINTF("ShowString:%s position:(%d,%d) size:(%d,%d) fcolor: %04X bcolor: %04X\r\n ",
					\
					stringin,
					x,
					y,
					sizex,
					sizey,
					colorf,
					colorb);
#endif
				ST7789_ShowString(x, y, stringin, colorf, colorb, sizey, 1);//显示字符串
			}
			else {
				//错误数据帧
				return FALSE;
			}
			pTemp++;	
		}
	}
	

	return TRUE;
}
//显示图片实现
//指令          参数id   参数图片id值  参数座标头  参数座标值x   参数座标值y       
//showpicture  I(0x49)   (0x00)		 P(0x50)   (0x00)(0x00) (0x00)(0x00)
//配置帧结束   
//A(0x41)
//73 68 6F 77 70 69 63 74 75 72 65 49 00 50 00 01 00 02 41 0d 0a
static uint8_t cli_showpicture(void *para, uint8_t len)
{
	uint8_t *pTemp;
	pTemp = (uint8_t *)para;
	
	u8 updating = 1;
	
	u16 x = 0;
	u16 y = 0;
	u8  id = 0;
	
	
	if ((0 < len) && (NULL != pTemp)) {
		while (updating && '\r' != *pTemp)
		{
			if ('I' == *pTemp) {
				//id帧传输
				pTemp++;
				id += (*pTemp);
			}
			else if ('P' == *pTemp) {
				//座标传输
				pTemp++;
				x += (*pTemp) << 8;
				pTemp++;
				x += (*pTemp);
				pTemp++;
				y += (*pTemp) << 8;
				pTemp++;
				y += (*pTemp);
			}
			else if ('A' == *pTemp) {
				//帧传输结束，可以显示
				updating = 0;
#ifdef commdebug
				PRINTF("ShowPicture: position:(%d,%d) id:%d \r\n ",
					\
					x,
					y,
					id);
#endif
				ICPX_Diplay_Image(x, y, ICOPYX_IMAGES[id]);
				//ST7789_Fill(startx, starty, stopx, stopy, color);
			}
			else {
				//错误数据帧
				return FALSE;
			}
			pTemp++;
		}
	}
	return TRUE;
}
//多重指令同步传输实现
//指令      参数命令   参数命令长度  参数命令内容  参数命令   参数命令长度  参数命令内容.....    
//multicmd  L(0x4C)  (0x00)		  Nx(0x00)	   L(0x4C) (0x00)		 Nx(0x00).....
//配置帧结束   
//A(0x41)
//
static uint8_t cli_multicmd(void *para, uint8_t len)
{
	uint8_t *pTemp;
	pTemp = (uint8_t *)para;
	u8 updating = 1;
	u8 length = 0;
	u8 commin[128];
	u16 i, j;
	u8 cmd_match = 0;
	
	u8 ParaLen;
	u8 *ParaAddr;
	
	if ((0 < len) && (NULL != pTemp)) {
		while (updating && '\r' != *pTemp)
		{
			if ('L' == *pTemp) {
				//命令帧传输（长度+数据）
				pTemp++;
				length = *pTemp;
				if (length == 0)
				{
					return TRUE;
				}
				memset(commin, '\0', length + 1);
				//接收一条指令
				for(int i = 0 ; i < length ; i++)
				{
					pTemp++;
					commin[i] = *pTemp;
					
				}
				//寻找目标命令
				for(i = 0 ; i < sizeof(CLI_Cmd) / sizeof(COMMAND_S) ; i++) {
					if (0 == strncmp((const char *)commin,
						(void *)CLI_Cmd[i].pCmd,
						strlen(CLI_Cmd[i].pCmd))) {
						//字符串匹配，也就是找到了目标指令
						cmd_match = 1;
						ParaLen = length - strlen(CLI_Cmd[i].pCmd);		//参数长度
						//PRINTF("CMD Length: %d", Handle.len);
						ParaAddr = &commin[strlen(CLI_Cmd[i].pCmd)];	//参数地址

						if(NULL != CLI_Cmd[i].pFun) {
							//调用目标函数，得到运行结果（1成功）
							if(CLI_Cmd[i].pFun(ParaAddr, ParaLen)) {
								//PRINTF("\r\n-> OK\r\n");
								length = 0;
								break;
							}
							else {
								//PRINTF("\r\n-> PARA. ERR\r\n");
								return FALSE;
							}			
						}
						else {
							//目标指令不存在（但是结构体规定了命令，即无效命令）
							//PRINTF("\r\n-> FUNC. ERR\r\n");
							return FALSE;
						}
					}
				}
				if (0 == cmd_match) {
					//没有匹配的指令
					//PRINTF("\r\n-> CMD ERR, try: help\r\n\r\n");
					return FALSE;
				}
				length = 0;
			}
			else if ('A' == *pTemp) {
				//帧传输结束，可以显示
				updating = 0;
#ifdef commdebug
				PRINTF("FillScreen:color: %04X \r\n ",
					\
					color);
#endif
			}
			else {
				//错误数据帧
				return FALSE;
			}
			pTemp++;
		}
	}
	return TRUE;
}
//设定屏幕亮度实现
//指令         参数bl   参数背光亮度 
//setbaklight  B(0x42) (0x00)	
//配置帧结束   
//A(0x41)
//
static uint8_t cli_setbaklight(void *para, uint8_t len)
{
	uint8_t *pTemp;
	pTemp = (uint8_t *)para;
	
	u8 updating = 1;
	
	u8 baklight = 0;
	
	
	if ((0 < len) && (NULL != pTemp)) {
		while (updating && '\r' != *pTemp)
		{
			if ('B' == *pTemp) {
				//背光亮度传输
				pTemp++;
				baklight += (*pTemp);
			}
			else if ('A' == *pTemp) {
				//帧传输结束，可以显示
				updating = 0;
#ifdef commdebug
				PRINTF("setbacklight:level: %04X \r\n ",
					\
					baklight);
#endif
				ST7789_SetBaklight(baklight);
			}
			else {
				//错误数据帧
				return FALSE;
			}
			pTemp++;
		}
	}
	return TRUE;
}
//目标存活指令实现
static uint8_t cli_targetalive(void *para, uint8_t len)
{
	setback();
}
//获取版本号指令实现
static uint8_t cli_getversion(void *para, uint8_t len)
{
	printf("#version:%d.%d.%d.%d",
		ICOPYX_SW_VERSION,
		ICOPYX_SW_SUBVERSION,
		ICOPYX_FW_VERSION,
		ICOPYX_FW_SUBVERSION);
	fflush(stdout);
	return TRUE;
}
//获取内部id指令实现
static uint8_t cli_getid(void *para, uint8_t len)
{
	u8 datatemp[10];
	memset(datatemp, 0, sizeof(datatemp));
	W25QXX_ReadUID(datatemp);
	u8 dataprint[42];
	memset(dataprint, 0, sizeof(dataprint));
	sprintf(dataprint,
		"%08lX%08lX%08lX%02X%02X%02X%02X%02X%02X%02X%02X", 
		(*(u32*)(0x1FFFF7E8)),
		(*(u32*)(0x1FFFF7EC)),
		(*(u32*)(0x1FFFF7F0)),
		datatemp[0], 
		datatemp[1], 
		datatemp[2], 
		datatemp[3], 
		datatemp[4], 
		datatemp[5], 
		datatemp[6], 
		datatemp[7]);
	printf("#theid:%s\r\n", dataprint);
	fflush(stdout);
	return TRUE;
}
//计划关机指令 注意这个指令直接调用SHUTDOWNMETH	,也就是不再会返回，
static uint8_t cli_planshutdown(void *para, uint8_t len)
{
	PRINTF("\r\n-> OK\r\n");//先释放远端调用
	SHUTDOWNMETH();
	return TRUE;
}
//终端初始化实现
void cli_init(uint32_t baud)
{
	uint8_t i;

	memset((uint8_t *)&cli_rx_buff, 0, sizeof(RX_BUFF_TYPE));

	USART_INIT(baud);

	/* init. every command */
	for (i = 0; i < sizeof(CLI_Cmd) / sizeof(COMMAND_S); i++) {
		/* need to init. or not */
		if (NULL != CLI_Cmd[i].pInit) {
			if (FALSE == CLI_Cmd[i].pInit()) {
				/* something wrong */
				PRINTF("\r\n-> FUN[%d] INIT WRONG\r\n", i);
			}
		}
	}

	//PRINTF(" \r\n");
	//PRINTF("CLI version: V0.2\r\n\r\n");
}
//接收处理（循环调用）
static void cli_rx_handle(RX_BUFF_TYPE *rx_buff)
{
	static HANDLE_TYPE_S Handle = { .len = 0 };
	uint8_t i = Handle.len;
	uint8_t ParaLen;
	uint8_t *ParaAddr;
	uint8_t cmd_match = FALSE;
	/*  ---------------------------------------
	    第1步：从缓冲区接收命令
	    ---------------------------------------*/
	while (1) {
		if (Handle.len < HANDLE_LEN) {
			//检察缓冲区是否满了
			if(TRUE == QUEUE_OUT((*rx_buff), Handle.buff[Handle.len])) 
			{
				//缓冲区有传入数据（单字节）放入缓冲区
				Handle.len++;
			}
			else {
				//已经把所有都传入了
					    /* display char in terminal */
					    //				for (; i < Handle.len; i++) {
					    //					USART_SendData(USART1, Handle.buff[i]);
					    //				}
			break;
			}
		}
		else {
			//缓冲区满了，不传了
			break;
		}
	}
	/*  ---------------------------------------
	    第2步：处理缓冲区里的命令，匹配指令并且运行
	    ---------------------------------------*/
	if ((2 == Handle.len) && (KEY_ENTER == Handle.buff[Handle.len - 2])\
		&& (KEY_NEWLINE == Handle.buff[Handle.len - 1])) {
		//首先判断是不是空命令，是就跳过
		Handle.len = 0;
	}
	else if (2 < Handle.len) {
		//检查长度大于两个字符，此时有数据传入
		if(KEY_ENTER == Handle.buff[Handle.len - 2] && (KEY_NEWLINE == Handle.buff[Handle.len - 1])) {
			//检查结尾为/r/n
			Handle.buff[Handle.len - 2] = '\0';
			Handle.buff[Handle.len - 1] = '\0';
			//做个字符串结尾，方便处理
			//寻找目标命令
			for(i = 0 ; i < sizeof(CLI_Cmd) / sizeof(COMMAND_S) ; i++) {
				if (0 == strncmp((const char *)Handle.buff,
					(void *)CLI_Cmd[i].pCmd,
					strlen(CLI_Cmd[i].pCmd))) {
					//字符串匹配，也就是找到了目标指令
					cmd_match = TRUE;
					ParaLen = Handle.len - strlen(CLI_Cmd[i].pCmd);		//参数长度
					//PRINTF("CMD Length: %d", Handle.len);
					ParaAddr = &Handle.buff[strlen(CLI_Cmd[i].pCmd)];	//参数地址

					if(NULL != CLI_Cmd[i].pFun) {
						//调用目标函数，得到运行结果（1成功）
						if(CLI_Cmd[i].pFun(ParaAddr, ParaLen)) {
							PRINTF("\r\n-> OK\r\n");
							Handle.len = 0;
						}
						else {
							PRINTF("\r\n-> PARA. ERR\r\n");
						}			
					}
					else {
						//目标指令不存在（但是结构体规定了命令，即无效命令）
						PRINTF("\r\n-> FUNC. ERR\r\n");
					}
				}
			}
			if (FALSE == cmd_match) {
				//没有匹配的指令
				PRINTF("\r\n-> CMD ERR, try: help\r\n\r\n");
			}
			Handle.len = 0;
		}
	}
	if (Handle.len >= HANDLE_LEN) {
		//缓冲区满了，从头开始
		Handle.len = 0;
	}
}
//发送处理（暂时不用）
static void cli_tx_handle(void)
{
	/* not used for now */
}
//循环处理调用
void cli_run(void)
{
	cli_rx_handle(&cli_rx_buff);
	//cli_tx_handle();
}
//中断接收，传入queue
void USART1_IRQHandler(void)
{
	uint8_t ucData;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		ucData = USART_ReceiveData(USART1);
		QUEUE_IN(cli_rx_buff, ucData);
	}
}


