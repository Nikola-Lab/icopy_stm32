#include "sys.h"
#include "usart.h"	  
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos 使用	  
#endif
#include "sys_queue.h"	  
#include "sys_command_line.h"	

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}
; 

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
//重定义_write函数 
int _write(int fd, char *pBuffer, int size)
{
	for (int i = 0; i < size; i++)
	{
		while ((USART1->SR & 0X40) == 0) ;	//等待上一次串口数据发送完成  
		USART1->DR = (u8) pBuffer[i];		//写DR,串口1将发送数据
	}
	return size;
}

#endif 

/*使用microLib的方法*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA = 0;       //接收状态标记	  
  
void uart_init(u32 bound) {
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);	//使能USART1，GPIOA时钟以及复用功能时钟
     //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
   
	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(USART1, ENABLE);                    //使能串口 

}
/*****************  send a char **********************/
void Usart_SendByte(USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* send a char */
	USART_SendData(pUSARTx, ch);

	/* wait */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET) ;
}

/****************** send a array ************************/
void Usart_SendArray(USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
	uint8_t i;

	for (i = 0; i < num; i++) {
		/* send a char */
		Usart_SendByte(pUSARTx, array[i]);

	}
	/* wait */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET) ;
}

/*****************  send a string **********************/
void Usart_SendString(USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k = 0;
	do {
		Usart_SendByte(pUSARTx, *(str + k));
		k++;
	} while (*(str + k) != '\0');

	/* wait */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET)
	{}
}

/*****************  send half word **********************/
void Usart_SendHalfWord(USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;

	/* get higher byte */
	temp_h = (ch & 0XFF00) >> 8;
	/* get lower byte */
	temp_l = ch & 0XFF;

	/* send higher byte */
	USART_SendData(pUSARTx, temp_h);
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET) ;

	/* send lower byte */
	USART_SendData(pUSARTx, temp_l);
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET) ;
}


//void USART1_IRQHandler(void)                	//串口1中断服务程序
//{
//	u8 Res;
//	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//		{
//			Res = USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
//		
//			if ((USART_RX_STA & 0x8000) == 0)//接收未完成
//			{
//				if (USART_RX_STA & 0x4000)//接收到了0x0d
//				{
//					if (Res != 0x0a)USART_RX_STA = 0;//接收错误,重新开始
//					else USART_RX_STA |= 0x8000;	//接收完成了 
//				}
//				else //还没收到0X0D
//				{	
//					if (Res == 0x0d)USART_RX_STA |= 0x4000;
//					else
//					{
//						USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;
//						USART_RX_STA++;
//						if (USART_RX_STA > (USART_REC_LEN - 1))USART_RX_STA = 0;//接收数据错误,重新开始接收	  
//					}		 
//				}
//			}   		 
//		} 
//} 
#endif	

