#include "AW87318.h"
#include "sys.h"
#include "ICOPYX_BSP.h"

void	AW87318_Init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin = AMP_STDN_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(AMP_STDN_GPIO_Port, &GPIO_InitStructure);
	GPIO_ResetBits(AMP_STDN_GPIO_Port, AMP_STDN_Pin);
	//PWR_BackupAccessCmd(DISABLE);/* 禁止修改RTC和后备寄存器*/
}

void	AW87318_START(void) {
	GPIO_SetBits(AMP_STDN_GPIO_Port, AMP_STDN_Pin);
}

void	AW87318_SLEEP(void) {
	GPIO_ResetBits(AMP_STDN_GPIO_Port, AMP_STDN_Pin);
}
	