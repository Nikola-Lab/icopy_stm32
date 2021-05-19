#include "debugflag.h"

u8 debug_state_flag(void){
	return (u8)((*(u32*)(0xE000EDF0)) & 0x01);
}
