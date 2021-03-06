#include "common.h"

extern uint8_t file_name[FILE_NAME_LENGTH];
uint8_t tab_1024[4096] =
{
    0
};

//通过串口接收一个文件
void SerialDownload(int files)
{
	uint8_t Number[10] = "          ";
	uint8_t WriteAddressStr[11] = "";
	uint8_t WStr[11] = "";
	int32_t WriteAddress;
	memcpy(WriteAddressStr, "\0", sizeof(WriteAddressStr));
	memcpy(WStr, "\0", sizeof(WStr));
    int32_t Size = 0;
	if (files == 2)
	{
		SerialPutString("Waiting for the Address to be write ... \r\n");
		GetInputString(WriteAddressStr);
		Str2Int(WriteAddressStr, &WriteAddress);
		SerialPutString("ok,address =");
		Int2Str(WStr, WriteAddress);
		SerialPutString(WStr);
		SerialPutString("\r\n");
	}
	//SerialPutString("Waiting for the file to be sent ... (press 'a' to abort)\r\n");
	if (files == 1)
	{
		Size = Ymodem_Receive(&tab_1024[0]);
	}
	else if (files == 2)
	{
		Size = Ymodem_Receive_To_Flash(&tab_1024[0],WriteAddress);
	}
    if (Size > 0)
    {
	    SerialPutString("\r\nUPDATE END!\r\n");
        //SerialPutString(file_name);
        //Int2Str(Number, Size);
        //SerialPutString("\n\r Size: ");
        //SerialPutString(Number);
        //SerialPutString(" Bytes\r\n");
        //SerialPutString("-------------------\n");
    }
    else if (Size == -1)
    {
	    SerialPutString("\r\nLARGE!\r\n");
    }
    else if (Size == -2)
    {
	    SerialPutString("\r\nVerification failed!\r\n");
    }
    else if (Size == -3)
    {
	    SerialPutString("\r\nAborted by user.\r\n");
    }
    else
    {
	    SerialPutString("\r\nFailed to receive the file!\r\n");
    }
}

/*******************************文件结束***************************************/
