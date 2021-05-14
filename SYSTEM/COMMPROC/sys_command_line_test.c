#include "sys_command_line.h"
#include "usart.h"
#include "ICOPYX_BSP.h"
//#define commdebug

static uint8_t cli_presspm3(void *para, uint8_t len);
static uint8_t cli_turnoffpm3(void *para, uint8_t len);
static uint8_t cli_turnonpm3(void *para, uint8_t len);
static uint8_t cli_restartpm3(void *para, uint8_t len);

static uint8_t cli_volbat(void *para, uint8_t len);
static uint8_t cli_volinput(void *para, uint8_t len);

static uint8_t cli_charge(void *para, uint8_t len);
static uint8_t cli_h3start(void *para, uint8_t len);
static uint8_t cli_lcd2h3(void *para, uint8_t len);
static uint8_t cli_lcd2st(void *para, uint8_t len);
static uint8_t cli_planshutdown(void *para, uint8_t len);
static uint8_t cli_butstart(void *para, uint8_t len);
static uint8_t cli_butstop(void *para, uint8_t len);
static uint8_t cli_flashtest(void *para, uint8_t len);
static uint8_t cli_turnonh3(void *para, uint8_t len);
static uint8_t cli_turnoffh3(void *para, uint8_t len);
static uint8_t cli_screentest(void *para, uint8_t len);
static uint8_t cli_audioon(void *para, uint8_t len);
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
	//pm3状态操作指令序列
	{ "presspm3", NULL, NULL, cli_presspm3},
	//pm3电源指令序列
	{ "turnonpm3", NULL, NULL, cli_turnonpm3},
	{ "turnoffpm3", NULL, NULL, cli_turnoffpm3},
	{ "restartpm3", NULL, NULL, cli_restartpm3},
	//电源电压指令序列
	{ "volbat", NULL, NULL, cli_volbat},
	{ "volvcc", NULL, NULL, cli_volinput},
	//查询充电状态指令序列(耗时问题)
	{ "charge", NULL, NULL, cli_charge},
	{ "h3start", NULL, NULL, cli_h3start},
	//转交屏幕权限指令
	{ "givemelcd", NULL, NULL, cli_lcd2h3},
	{ "giveyoulcd", NULL, NULL, cli_lcd2st},
	//休眠命令
	{ "plan2sleep", NULL, NULL, cli_planshutdown },
	//启动关闭按钮测试
	{ "buttest_start", NULL, NULL, cli_butstart},
	{ "buttest_stop", NULL, NULL, cli_butstop},
	
	{ "screentest_start", NULL, NULL, cli_screentest},
	
	{ "flashtest", NULL, NULL, cli_flashtest},
	
	{ "turnonh3", NULL, NULL, cli_turnonh3},
	{ "turnoffh3", NULL, NULL, cli_turnoffh3},
	{ "turnonamp", NULL, NULL, cli_audioon},
	
	
};

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
	printf("#batvol:%d",BATvolavl);
	fflush(stdout);
	return TRUE;
}
//读取电源电压实现
static uint8_t cli_volinput(void *para, uint8_t len)
{
	printf("#vccvol:%d",VCCvolavl);
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
	return TRUE;
}	
//lcd转交给h3指令
static uint8_t cli_lcd2h3(void *para, uint8_t len)
{
	SPISELH3();
	return TRUE;
}

//lcd转交给st指令
static uint8_t cli_lcd2st(void *para, uint8_t len)
{
	SPISELST();
	return TRUE;
}
//计划关机指令
static uint8_t cli_planshutdown(void *para, uint8_t len)
{
	SETSHUTDOWNFLAG();
	return TRUE;
}
static uint8_t cli_butstart(void *para, uint8_t len)
{
	testkeying = 1;
	return TRUE;
}
static uint8_t cli_butstop(void *para, uint8_t len)
{
	testkeying = 0;
	return TRUE;
}
static uint8_t cli_flashtest(void *para, uint8_t len)
{
	KFS_POWERON_SEARCH();		//维护文件系统，重建内部文件信息缓存
	return ICPX_img_data_ok == 1 ? TRUE : FALSE;
}
static uint8_t cli_turnonh3(void *para, uint8_t len)
{
	turnonh3();
	return TRUE;
}
static uint8_t cli_turnoffh3(void *para, uint8_t len)
{
	turnoffh3();
	return TRUE;
}
static uint8_t cli_screentest(void *para, uint8_t len)
{
	ST7789_BL_ON();
	return TRUE;
}
static uint8_t cli_audioon(void *para, uint8_t len)
{
	ICPX_AMP_Init();			//初始化音频放大器
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


