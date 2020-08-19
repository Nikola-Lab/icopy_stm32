#include "common.h"
#include "ymodem.h"
#include "BspTime.h"

pFunction Jump_To_Application;
uint32_t JumpAddress;
uint32_t BlockNbr = 0, UserMemoryMask = 0;
__IO uint32_t FlashProtection = 0;
extern uint32_t FlashDestination;

//整形数据转到字符串
void Int2Str(uint8_t* str, int32_t intnum)
{
    uint32_t i, Div = 1000000000, j = 0, Status = 0;

    for (i = 0; i < 10; i++)
    {
        str[j++] = (intnum / Div) + 48;

        intnum = intnum % Div;
        Div /= 10;
        if ((str[j-1] == '0') & (Status == 0))
        {
            j = 0;
        }
        else
        {
            Status++;
        }
    }
}

//字符串转到数据
uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum)
{
    uint32_t i = 0, res = 0;
    uint32_t val = 0;

    if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
    {
        if (inputstr[2] == '\0')
        {
            return 0;
        }
        for (i = 2; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                //返回1
                res = 1;
                break;
            }
            if (ISVALIDHEX(inputstr[i]))
            {
                val = (val << 4) + CONVERTHEX(inputstr[i]);
            }
            else
            {
                //无效输入返回0
                res = 0;
                break;
            }
        }

        if (i >= 11)
        {
            res = 0;
        }
    }
    else//最多10为2输入
    {
        for (i = 0; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                //返回1
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
            {
                val = val << 10;
                *intnum = val;
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
            {
                val = val << 20;
                *intnum = val;
                res = 1;
                break;
            }
            else if (ISVALIDDEC(inputstr[i]))
            {
                val = val * 10 + CONVERTDEC(inputstr[i]);
            }
            else
            {
                //无效输入返回0
                res = 0;
                break;
            }
        }
        //超过10位无效，返回0
        if (i >= 11)
        {
            res = 0;
        }
    }

    return res;
}

uint32_t GetIntegerInput(int32_t * num)
{
    uint8_t inputstr[16];

    while (1)
    {
        GetInputString(inputstr);
        if (inputstr[0] == '\0') continue;
        if ((inputstr[0] == 'a' || inputstr[0] == 'A') && inputstr[1] == '\0')
        {
            SerialPutString("User Cancelled \r\n");
            return 0;
        }

        if (Str2Int(inputstr, num) == 0)
        {
            SerialPutString("Error, Input again: \r\n");
        }
        else
        {
            return 1;
        }
    }
}

uint32_t SerialKeyPressed(uint8_t *key)
{

    if ( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
    {
        *key = (uint8_t)USART1->DR;
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t GetKey(void)
{
    uint8_t key = 0;

    //等待按键按下
    while (IS_TIMEOUT_1MS(eTim2, 3000))
    {
	    if (SerialKeyPressed((uint8_t*)&key)) 
	    {
		    break;
	    }
    }
	g_Tim2Array[eTim2] = 0;
    return key;

}

//串口发送一个字符
void SerialPutChar(uint8_t c)
{
    USART_SendData(USART1, c);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
    }
}

//串口发送一个字符串
void Serial_PutString(uint8_t *s)
{
    while (*s != '\0')
    {
        SerialPutChar(*s);
        s++;
    }
}

//从串口获取一个字符串
void GetInputString (uint8_t * buffP)
{
    uint32_t bytes_read = 0;
    uint8_t c = 0;
    do
    {
        c = GetKey();
        if (c == '\r')
            break;
        if (c == '\b')// Backspace 按键  
        {
            if (bytes_read > 0)
            {
                SerialPutString("\b \b");
                bytes_read --;
            }
            continue;
        }
        if (bytes_read >= CMD_STRING_SIZE )
        {
            SerialPutString("Command string size overflow\r\n");
            bytes_read = 0;
            continue;
        }
        if (c >= 0x20 && c <= 0x7E)
        {
            buffP[bytes_read++] = c;
            SerialPutChar(c);
        }
    }
    while (1);
    SerialPutString(("\n\r"));
    buffP[bytes_read] = '\0';
}

//计算Falsh页
//输入参数：Size:文件长度
uint32_t FLASH_PagesMask(__IO uint32_t Size)
{
    uint32_t pagenumber = 0x0;
    uint32_t size = Size;

    if ((size % PAGE_SIZE) != 0)
    {
        pagenumber = (size / PAGE_SIZE) + 1;
    }
    else
    {
        pagenumber = size / PAGE_SIZE;
    }
    return pagenumber;

}

//关闭Flash写保护
void FLASH_DisableWriteProtectionPages(void)
{
    uint32_t useroptionbyte = 0, WRPR = 0;
    uint16_t var1 = OB_IWDG_SW, var2 = OB_STOP_NoRST, var3 = OB_STDBY_NoRST;
    FLASH_Status status = FLASH_BUSY;

    WRPR = FLASH_GetWriteProtectionOptionByte();

    //测试是否写保护
    if ((WRPR & UserMemoryMask) != UserMemoryMask)
    {
        useroptionbyte = FLASH_GetUserOptionByte();

        UserMemoryMask |= WRPR;

        status = FLASH_EraseOptionBytes();

        if (UserMemoryMask != 0xFFFFFFFF)
        {
            status = FLASH_EnableWriteProtection((uint32_t)~UserMemoryMask);
        }
        //用处选项字是否有编程
        if ((useroptionbyte & 0x07) != 0x07)
        {
            //重新保存选项字
            if ((useroptionbyte & 0x01) == 0x0)
            {
                var1 = OB_IWDG_HW;
            }
            if ((useroptionbyte & 0x02) == 0x0)
            {
                var2 = OB_STOP_RST;
            }
            if ((useroptionbyte & 0x04) == 0x0)
            {
                var3 = OB_STDBY_RST;
            }

            FLASH_UserOptionByteConfig(var1, var2, var3);
        }

        if (status == FLASH_COMPLETE)
        {
            SerialPutString("Write Protection disabled...\r\n");

            SerialPutString("...and a System Reset will be generated to re-load the new option bytes\r\n");
            //系统复位重新加载选项字
            NVIC_SystemReset();
        }
        else
        {
            SerialPutString("Error: Flash write unprotection failed...\r\n");
        }
    }
    else
    {
        SerialPutString("Flash memory not write protected\r\n");
    }
}

void Main_Menu(void)
{
    uint8_t key = 0;
    static uint8_t uStatus = 0;
    BlockNbr = (FlashDestination - 0x08000000) >> 12;

 
#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)
    UserMemoryMask = ((uint32_t)~((1 << BlockNbr) - 1));
#else /* USE_STM3210E_EVAL */
    if (BlockNbr < 62)
    {
        UserMemoryMask = ((uint32_t)~((1 << BlockNbr) - 1));
    }
    else
    {
        UserMemoryMask = ((uint32_t)0x80000000);
    }
#endif /* (STM32F10X_MD) || (STM32F10X_MD_VL) */

    if ((FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask)
    {
        FlashProtection = 1;
    }
    else
    {
        FlashProtection = 0;
    }
	g_Tim2Array[eTim1] = 0;
    while (IS_TIMEOUT_1MS(eTim1, 100))
    {
       switch(uStatus)
        {
            case 0:
	        SerialPutString("\r\n*********************************************************\r\n");
            SerialPutString( "1.update app\r\n");  
            SerialPutString( "2.start app\r\n");
	        SerialPutString("3.update spi flash\r\n");
            SerialPutString( "*********************************************************\r\n");
            SerialPutString( "choose:\r\n");
            uStatus = 1;
            break;
            case 1:
			SerialPutString( "......\r\n");//
	        while (1)
	        {
            key = GetKey();
            if (key == 0x31)
            {
	            FLASH_Unlock();
	            //FLASH_ReadOutProtection(DISABLE);
                /* Download user application in the Flash */
                SerialDownload(1);
		        //FLASH_ReadOutProtection(ENABLE);  
	            SerialPutString("\rdownload app end\n\r");//
            }
            else if (key == 0x32)
            {
                SerialPutString("waiting...\r\n");
	            
	            g_Tim2Array[eTim2] = 0;
	            while (IS_TIMEOUT_1MS(eTim2, 10000))
	            {
	            }
	            SerialPutString("sleep!\r\n");
	            //延时，等待h3关机
                BspTim2Close();
                JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);

                /* Jump to user application */
                Jump_To_Application = (pFunction) JumpAddress;
                /* Initialize user application's Stack Pointer */
                __set_MSP(*(__IO uint32_t*) ApplicationAddress);
                Jump_To_Application();
            }
	        else if (key == 0x33)
	        {
		        SerialPutString("starting spi flash update\r\n");
		        SerialPutString(" \r\n");
		        /* Download user application in the Flash */
		        SerialDownload(2);
		        SerialPutString("\rdownload flash end\n\r");//
	        } 
					
			}

            default:
            break;
        } 
//        else
//        {
//            if (FlashProtection == 0)
//            {
//                SerialPutString("Invalid Number ! ==> The number should be either 1, 2 or 3\r");
//            }
//            else
//            {
//                SerialPutString("Invalid Number ! ==> The number should be either 1, 2, 3 or 4\r");
//            }
//        }
    }
}


/*******************************文件结束***************************************/
