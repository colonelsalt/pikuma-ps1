#include <libetc.h>
#include <libgpu.h>
#include <libgte.h>

#include "util.h"

constexpr u32 VIDEO_MODE = 0;
constexpr u32 SCREEN_RES_X = 320;
constexpr u32 SCREEN_RES_Y = 240;
constexpr u32 SCREEN_CENTRE_X = SCREEN_RES_X >> 1;
constexpr u32 SCREEN_CENTRE_Y = SCREEN_RES_Y >> 1;
constexpr u32 SCREEN_Z = 400;

constexpr u32 OT_LENGTH = 16;
constexpr u32 PRIMBUFF_LENGTH = 2'048;

struct double_buffer
{
	// One man's draw buffer is another man's display buffer
	DRAWENV Draw[2];
	DISPENV Disp[2];
};

double_buffer g_Screen;
u16 g_CurrentBuffer; // 0 or 1 depending on which of the two draw buffers is in front

u32 g_OrderingTable[2][OT_LENGTH];

u8 g_PrimBuff[2][PRIMBUFF_LENGTH];
u8* g_NextPrim;

POLY_F3* g_Triangle;
TILE* g_Tile;
POLY_G4* g_Quad;

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
	SetGeomScreen(SCREEN_Z);

	// Enable display
	SetDispMask(1);
}

void DisplayFrame()
{
	DrawSync(0);
	VSync(0);

	PutDispEnv(g_Screen.Disp + g_CurrentBuffer);
	PutDrawEnv(g_Screen.Draw + g_CurrentBuffer);

	// Draw all primitives in ordering table..?!
	DrawOTag(g_OrderingTable[g_CurrentBuffer] + OT_LENGTH - 1);

	g_CurrentBuffer = !g_CurrentBuffer;
	g_NextPrim = g_PrimBuff[g_CurrentBuffer];
}

void Setup()
{
	ScreenInit();

	g_NextPrim = g_PrimBuff[g_CurrentBuffer];
}

static u32 s_X = 82;
static b32 s_GoingRight = true;
void Update()
{
	// Clear ordering table
	ClearOTagR(g_OrderingTable[g_CurrentBuffer], OT_LENGTH);

	// Filling in ordering table with primitives

	g_Tile = (TILE*)g_NextPrim;
	setTile(g_Tile);
	setXY0(g_Tile, s_X, 32); // top-left pos
	setWH(g_Tile, 64, 64); // width/height
	setRGB0(g_Tile, 0, 255, 0); // tile colour
	addPrim(g_OrderingTable[g_CurrentBuffer], g_Tile);
	g_NextPrim += sizeof(TILE);

	if (s_GoingRight)
	{
		s_X++;
		if (s_X >= SCREEN_RES_X - 64)
		{
			s_GoingRight = false;
		}
	}
	else
	{
		s_X--;
		if (s_X <= 0)
		{
			s_GoingRight = true;
		}
	}

	g_Triangle = (POLY_F3*)g_NextPrim;
	setPolyF3(g_Triangle);
	setXY3(g_Triangle,
		    64, 100,
		   200, 150,
		    50, 220); // set vertices
	setRGB0(g_Triangle, 255, 0, 255);
	addPrim(g_OrderingTable[g_CurrentBuffer], g_Triangle);
	g_NextPrim += sizeof(POLY_F3);

	g_Quad = (POLY_G4*)g_NextPrim;
	setPolyG4(g_Quad);
	setXY4(g_Quad,
		   200, 150,
		   319, 200,
		   197,  82,
		   285,  75);
	setRGB0(g_Quad, 248, 221, 15); // yellow
	setRGB1(g_Quad, 7, 184, 111); // cyan
	setRGB2(g_Quad, 197, 64, 17); // green
	setRGB3(g_Quad, 255, 255, 255); // white
	addPrim(g_OrderingTable[g_CurrentBuffer], g_Quad);
	g_NextPrim += sizeof(POLY_G4);

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
