#include "joypad.h"

#include <libetc.h>

static u32 s_PadState;

void JoypadInit()
{
	PadInit(0);
}

void JoypadReset()
{
	s_PadState = 0;
}

void JoypadUpdate()
{
	s_PadState = PadRead(0);
}


b32 JoypadCheck(u32 Button)
{
	b32 Result = Button & s_PadState;
	return Result;
}