#include <libetc.h>
#include <libgpu.h>
#include <libgte.h>

#include "util.h"

constexpr u32 VIDEO_MODE = 0;
constexpr u32 SCREEN_RES_X = 320;
constexpr u32 SCREEN_RES_Y = 240;
constexpr u32 SCREEN_CENTRE_X = SCREEN_RES_X >> 1;
constexpr u32 SCREEN_CENTRE_Y = SCREEN_RES_Y >> 1;

struct double_buffer
{
	// One man's draw buffer is another man's display buffer
	DRAWENV Draw[2];
	DISPENV Disp[2];
};

double_buffer g_Screen;
u16 g_CurrentBuffer; // 0 or 1 depending on which of the two draw buffers is in front

void ScreenInit()
{
	// Reset GPU
	ResetGraph(0);

	// Set display area of first buffer
	SetDefDispEnv(&g_Screen.Disp[0], 0,            0, SCREEN_RES_X, SCREEN_RES_Y);
	SetDefDrawEnv(&g_Screen.Draw[0], 0, SCREEN_RES_Y, SCREEN_RES_X, SCREEN_RES_Y);

	SetDefDispEnv(&g_Screen.Disp[1], 0, SCREEN_RES_Y, SCREEN_RES_X, SCREEN_RES_Y);
	SetDefDrawEnv(&g_Screen.Draw[1], 0,            0, SCREEN_RES_X, SCREEN_RES_Y);

	// Assign each drawing buffer
	g_Screen.Draw[0].isbg = true;
	g_Screen.Draw[1].isbg = true;

	setRGB0(&g_Screen.Draw[0], 15, 13, 106);
	setRGB0(&g_Screen.Draw[1], 15, 13, 106);

	// Set initial draw+display buffer
	g_CurrentBuffer = 0;
	PutDispEnv(g_Screen.Disp + g_CurrentBuffer);
	PutDrawEnv(g_Screen.Draw + g_CurrentBuffer);

	// Initialise GTE
	InitGeom();
	SetGeomOffset(SCREEN_CENTRE_X, SCREEN_CENTRE_Y);
	SetGeomScreen(SCREEN_CENTRE_X);

	// Enable display
	SetDispMask(1);
}

void DisplayFrame()
{
	DrawSync(0);
	VSync(0);

	PutDispEnv(g_Screen.Disp + g_CurrentBuffer);
	PutDrawEnv(g_Screen.Draw + g_CurrentBuffer);

	g_CurrentBuffer = !g_CurrentBuffer;
}

void Setup()
{
	ScreenInit();
}

void Update()
{

}

void Render()
{
	DisplayFrame();
}

int main(void)
{    
	Setup();

	while (true)
	{
		Update();
		Render();
	}

    return 0;
}
