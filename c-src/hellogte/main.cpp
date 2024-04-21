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

constexpr u32 OT_LENGTH = 256;
constexpr u32 PRIMBUFF_LENGTH = 2'048;

struct double_buffer
{
	// One man's draw buffer is another man's display buffer
	DRAWENV Draw[2];
	DISPENV Disp[2];
};

SVECTOR g_CubeVertices[] =
{
	{ -128, -128, -128 },
	{  128, -128, -128 },
	{  128, -128,  128 },
	{ -128, -128,  128 },
	{ -128,  128, -128 },
	{  128,  128, -128 },
	{  128,  128,  128 },
	{ -128,  128,  128 }
};

constexpr u32 NUM_VERTICES = ArrayCount(g_CubeVertices);

u16 g_CubeFaces[] =
{
	0, 3, 2, // top
	0, 2, 1, // top
	4, 0, 1, // front
	4, 1, 5, // front
	7, 4, 5, // bottom
	7, 5, 6, // bottom
	5, 1, 2, // right
	5, 2, 6, // right
	2, 3, 7, // back
	2, 7, 6, // back
	0, 4, 7, // left
	0, 7, 3  // left
};

constexpr u32 NUM_FACES = ArrayCount(g_CubeFaces) / 3;

double_buffer g_Screen;
u16 g_CurrentBuffer; // 0 or 1 depending on which of the two draw buffers is in front

u32 g_OrderingTable[2][OT_LENGTH];

u8 g_PrimBuff[2][PRIMBUFF_LENGTH];
u8* g_NextPrim;

POLY_G3* g_Triangle;

SVECTOR g_Rotation = { 0, 0, 0 };
VECTOR g_Translation = { 0, 0, 900 };
VECTOR g_Scale = { ONE, ONE, ONE };

static MATRIX s_WorldMatrix;

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

void Update()
{
	// Clear ordering table
	ClearOTagR(g_OrderingTable[g_CurrentBuffer], OT_LENGTH);

	RotMatrix(&g_Rotation, &s_WorldMatrix); // populate matrix with rotation values
	TransMatrix(&s_WorldMatrix, &g_Translation); // populate matrix with translation values
	ScaleMatrix(&s_WorldMatrix, &g_Scale); // populate matrix with scale values

	SetRotMatrix(&s_WorldMatrix); // sets rot/scale matrix to be used by the GTE (for upcoming RotTransPers call)
	SetTransMatrix(&s_WorldMatrix); // sets translation matrix to be used by GTE

	for (u32 i = 0; i < NUM_FACES * 3; i += 3)
	{
		g_Triangle = (POLY_G3*)g_NextPrim;
		setPolyG3(g_Triangle);
		setRGB0(g_Triangle, 255,   0, 255);
		setRGB1(g_Triangle, 255, 255,   0);
		setRGB2(g_Triangle,   0, 255, 255);

		s32 OrderingTableZ, P, Flag;

		s32 NormalClip = RotAverageNclip3(&g_CubeVertices[g_CubeFaces[i]],
										  &g_CubeVertices[g_CubeFaces[i + 1]],
										  &g_CubeVertices[g_CubeFaces[i + 2]],
										  (s32*)&g_Triangle->x0,
										  (s32*)&g_Triangle->x1,
										  (s32*)&g_Triangle->x2,
										  &P, &OrderingTableZ, &Flag);

		// if we've gotten a sensible Z value, and the normal isn't facing away from us
		if (NormalClip > 0 && OrderingTableZ > 0 && OrderingTableZ < OT_LENGTH)
		{
			addPrim(g_OrderingTable[g_CurrentBuffer][OrderingTableZ], g_Triangle);
			g_NextPrim += sizeof(POLY_G3);
		}
	}

	g_Rotation.vx += 6;
	g_Rotation.vy += 8;
	g_Rotation.vz += 12;
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
