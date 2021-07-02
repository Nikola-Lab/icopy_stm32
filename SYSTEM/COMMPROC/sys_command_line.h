#ifndef __SYS_COMMAND_LINE_H
#define __SYS_COMMAND_LINE_H


#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "sys_queue.h"


#define CLI_ENABLE          TRUE                    //命令行是否启用
#define CLI_HISTORY         FALSE                   //命令历史是否启用
#define HISTORY_MAX         10                      //最大历史数量
#define CLI_PRINTF          TRUE                    //debugprintf是否启用
#define CLI_BAUDRATE        ((uint32_t)115200)      //默认波特率
#define USART_INIT(baud)    uart_init(baud)         //重定义初始化
#define SYSTEM_REBOOT()     NVIC_SystemReset()      //重启芯片


#define KEY_UP              "\x1b\x5b\x41"  //上键
#define KEY_DOWN            "\x1b\x5b\x42"  //下键
#define KEY_RIGHT           "\x1b\x5b\x43"  //右键
#define KEY_LEFT            "\x1b\x5b\x44"  //左键
#define KEY_ENTER           '\r'            //退格
#define KEY_NEWLINE         '\n'            //换行
#define KEY_BACKSPACE       '\b'            //空格

typedef  void(*pFunction)(void);

#if CLI_ENABLE
    #define CLI_INIT(...)       cli_init(__VA_ARGS__)
    #define CLI_RUN(...)        cli_run(__VA_ARGS__)
#else
    #define CLI_INIT(...)       ;
    #define CLI_RUN(...)        ;
#endif /* CLI_ENABLE */


#if CLI_PRINTF
    #define PRINTF(...)         printf(__VA_ARGS__);\
                                fflush(stdout);	//gnugcc特性
    #define SPRINTF(...)        sprintf(__VA_ARGS__)

enum {
    E_FONT_BLACK,
    E_FONT_L_RED,
    E_FONT_RED,
    E_FONT_GREEN,
    E_FONT_YELLOW,
    E_FONT_BLUE,
    E_FONT_PURPLE,
    E_FONT_CYAN,
    E_FONT_WHITE,
};

    #define PRINTF_COLOR(c, ...)    do {                            \
                                        switch (c) {                \
                                            case E_FONT_BLACK:      \
                                            TERMINAL_FONT_BLACK();  \
                                            break;                  \
                                            case E_FONT_L_RED:      \
                                            TERMINAL_FONT_L_RED();  \
                                            break;                  \
                                            case E_FONT_RED:        \
                                            TERMINAL_FONT_RED();    \
                                            break;                  \
                                            case E_FONT_GREEN:      \
                                            TERMINAL_FONT_GREEN();  \
                                            break;                  \
                                            case E_FONT_YELLOW:     \
                                            TERMINAL_FONT_YELLOW(); \
                                            break;                  \
                                            case E_FONT_BLUE:       \
                                            TERMINAL_FONT_BLUE();   \
                                            break;                  \
                                            case E_FONT_PURPLE:     \
                                            TERMINAL_FONT_PURPLE(); \
                                            break;                  \
                                            case E_FONT_CYAN:       \
                                            TERMINAL_FONT_CYAN();   \
                                            break;                  \
                                            case E_FONT_WHITE:      \
                                            TERMINAL_FONT_WHITE();  \
                                            break;                  \
                                        }                           \
                                        printf(__VA_ARGS__);        \
                                        TERMINAL_FONT_GREEN();      \
                                    } while(0)
#else
    #define PRINTF(...)         ;
    #define SPRINTF(...)        ;
    #define PRINTF_COLOR(c, ...);
#endif /* CLI_PRINTF */




/* assert---------------------------------------------------------------BEGIN */

/* #val：使用#把宏参数变为一个字符串,用##把两个宏参数贴合在一起 */
#define ASSERT(val)                                                     \
if (!(val))                                                             \
{                                                                       \
    PRINTF("(%s) has assert failed at %s.\n", #val, __FUNCTION__);      \
    while (1);                                                          \
}

/* assert-----------------------------------------------------------------END */




/* debug----------------------------------------------------------------BEGIN */

#define ERR(fmt)        do {                                            \
                            TERMINAL_FONT_RED();                        \
                            PRINTF("### ERROR ###: "fmt"\r\n");         \
                            TERMINAL_FONT_GREEN();                      \
                        } while(0)
#define ERRA(fmt, ...)  do {                                            \
                            TERMINAL_FONT_RED();                        \
                            PRINTF("### ERROR ### %s(%d): "fmt"\r\n",   \
                                __FUNCTION__, __LINE__, __VA_ARGS__);   \
                            TERMINAL_FONT_GREEN();                      \
                        }while(0)
#define LOG(fmt)        do {                                            \
                            TERMINAL_FONT_CYAN();                       \
                            PRINTF("[Log]: "fmt"\r\n");                 \
                            TERMINAL_FONT_GREEN();                      \
                        } while(0)
#define LOGA(fmt, ...)  do {                                            \
                            TERMINAL_FONT_CYAN();                       \
                            PRINTF("[Log]: "fmt"\r\n", __VA_ARGS__);    \
                            TERMINAL_FONT_GREEN();                      \
                        } while(0)
#define DBG(fmt)        do {                                            \
                            TERMINAL_FONT_YELLOW();                     \
                            PRINTF("[Debug] %s(%d): "fmt"\r\n",         \
                                __FUNCTION__, __LINE__);                \
                            TERMINAL_FONT_GREEN();                      \
                        } while(0)
#define DBGA(fmt, ...)  do {                                            \
                            TERMINAL_FONT_YELLOW();                     \
                            PRINTF("[Debug] %s(%d): "fmt"\r\n",         \
                                __FUNCTION__, __LINE__, __VA_ARGS__);   \
                            TERMINAL_FONT_GREEN();                      \
                        } while(0)
#define DIE(fmt)        do {                                            \
                            TERMINAL_FONT_RED();                        \
                            TERMINAL_HIGH_LIGHT();                      \
                            PRINTF("### DIE ### %s(%d): "fmt"\r\n",     \
                                __FUNCTION__, __LINE__);                \
                        } while(1) /* infinite loop */
#define NL1()           do { PRINTF("\r\n"); } while(0)
#define NL2()           do { PRINTF("\r\n\r\n"); } while(0)
#define NL3()           do { PRINTF("\r\n\r\n\r\n"); } while(0)

/* debug------------------------------------------------------------------END */

#define RX_BUFF_TYPE        QUEUE1024_S
extern RX_BUFF_TYPE         cli_rx_buff;


 typedef struct {
    const char *pCmd;
    const char *pHelp;
    uint8_t (*pInit)(void);
    uint8_t (*pFun)(void *, uint8_t);
} COMMAND_S;



extern void cli_init(uint32_t baud);
extern void cli_task(void);
void cli_run(void);
void USART1_IRQHandler(void);
#endif /* __SYS_COMMAND_LINE_H */

