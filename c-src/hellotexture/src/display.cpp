#include "display.h"

#include <libetc.h>
#include "globals.h"


static double_buffer s_Screen;
static u16 s_CurrentBuffer; // 0 or 1 depending on which of the two draw buffers is in front

u16 GetCurrBuff()
{
	return s_CurrentBuffer;
}

void ScreenInit()
{
	// Reset GPU
	ResetGraph(0);

	// Set display area of first buffer
	SetDefDispEnv(&s_Screen.Disp[0], 0,            0, SCREEN_RES_X, SCREEN_RES_Y);
	SetDefDrawEnv(&s_Screen.Draw[0], 0, SCREEN_RES_Y, SCREEN_RES_X, SCREEN_RES_Y);

	SetDefDispEnv(&s_Screen.Disp[1], 0, SCREEN_RES_Y, SCREEN_RES_X, SCREEN_RES_Y);
	SetDefDrawEnv(&s_Screen.Draw[1], 0,            0, SCREEN_RES_X, SCREEN_RES_Y);

	// Assign each drawing buffer
	s_Screen.Draw[0].isbg = true;
	s_Screen.Draw[1].isbg = true;

	setRGB0(&s_Screen.Draw[0], 15, 13, 106);
	setRGB0(&s_Screen.Draw[1], 15, 13, 106);

	// Set initial draw+display buffer
	s_CurrentBuffer = 0;
	PutDispEnv(s_Screen.Disp + s_CurrentBuffer);
	PutDrawEnv(s_Screen.Draw + s_CurrentBuffer);

	// Initialise GTE
	InitGeom();
	SetGeomOffset(SCREEN_CENTRE_X, SCREEN_CENTRE_Y);
	SetGeomScreen(SCREEN_Z);

	// Enable display
	SetDispMask(1);
}

void DisplayFrame()
{
	DrawSync(0);
	VSync(0);

	PutDispEnv(s_Screen.Disp + s_CurrentBuffer);
	PutDrawEnv(s_Screen.Draw + s_CurrentBuffer);

	// Draw all primitives in ordering table..?!
	u32* OrderingTableEnd = GetOtAt(s_CurrentBuffer, OT_LENGTH - 1);
	DrawOTag(OrderingTableEnd);

	s_CurrentBuffer = !s_CurrentBuffer;
	ResetNextPrim(s_CurrentBuffer);
}