#include <libetc.h>
#include <libgpu.h>
#include <libgte.h>
#include "inline_n.h"

#include "util.h"

constexpr u32 VIDEO_MODE = 0;
constexpr u32 SCREEN_RES_X = 320;
constexpr u32 SCREEN_RES_Y = 240;
constexpr u32 SCREEN_CENTRE_X = SCREEN_RES_X >> 1;
constexpr u32 SCREEN_CENTRE_Y = SCREEN_RES_Y >> 1;
constexpr u32 SCREEN_Z = 400;

constexpr u32 OT_LENGTH = 2'048;
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
	3, 2, 0, 1,  // top
	0, 1, 4, 5,  // front
	4, 5, 7, 6,  // bottom
	1, 2, 5, 6,  // right
	2, 3, 6, 7,  // back
	3, 0, 7, 4,  // left
};

constexpr u32 NUM_FACES = ArrayCount(g_CubeFaces) / 4;

SVECTOR g_FloorVertices[] =
{
	{ -300, -300,   0 },
	{  300,  300,   0 },
	{ -300,  300,   0 },
	{  300, -300,   0 }
};

u16 g_FloorFaces[] =
{
	0, 1, 2,
	0, 3, 1
};

double_buffer g_Screen;
u16 g_CurrentBuffer; // 0 or 1 depending on which of the two draw buffers is in front

u32 g_OrderingTable[2][OT_LENGTH];

u8 g_PrimBuff[2][PRIMBUFF_LENGTH];
u8* g_NextPrim;

POLY_G4* g_Quad;
POLY_F3* g_Triangle;

u32 g_PadState;

SVECTOR g_CubeRotation = { 0, 0, 0 };
SVECTOR g_FloorRotation = { 0, 0, 0 };
VECTOR g_Scale = { ONE, ONE, ONE };

static MATRIX s_WorldMatrix;

VECTOR g_Velocity = { 0, 0, 0 };
VECTOR g_Acceleration = { 0, 0, 0 };
VECTOR g_CubePosition = { 0, 0, 0 };


VECTOR g_FloorPosition = { 0, 0, 0 };

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

	PadInit(0);

	g_NextPrim = g_PrimBuff[g_CurrentBuffer];

	g_Acceleration.vy = 1;

	g_CubePosition.vy = -400;
	g_CubePosition.vz = 1'800;

	g_FloorPosition.vy = 520;
	g_FloorPosition.vz = 2'000;

	g_FloorRotation.vx = -(1 << 12) / 4;
}

void Update()
{
	// Clear ordering table
	ClearOTagR(g_OrderingTable[g_CurrentBuffer], OT_LENGTH);

	u32 NewPadState = PadRead(0);
	g_PadState = NewPadState & (NewPadState ^ g_PadState);

	g_Velocity.vx += g_Acceleration.vx;
	g_Velocity.vy += g_Acceleration.vy;
	g_Velocity.vz += g_Acceleration.vz;

	g_CubePosition.vx += g_Velocity.vx >> 1;
	g_CubePosition.vy += g_Velocity.vy >> 1;
	g_CubePosition.vz += g_Velocity.vz >> 1;

	if (g_PadState & _PAD(0, PADLleft))
	{
		g_CubeRotation.vy += 20;
	}
	if (g_PadState & _PAD(0, PADLright))
	{
		g_CubeRotation.vy -= 20;
	}

	if (g_CubePosition.vy > 400)
	{
		g_Velocity.vy *= -1;
	}

	RotMatrix(&g_CubeRotation, &s_WorldMatrix); // populate matrix with rotation values
	TransMatrix(&s_WorldMatrix, &g_CubePosition); // populate matrix with translation values
	ScaleMatrix(&s_WorldMatrix, &g_Scale); // populate matrix with scale values

	SetRotMatrix(&s_WorldMatrix); // sets rot/scale matrix to be used by the GTE (for upcoming RotTransPers call)
	SetTransMatrix(&s_WorldMatrix); // sets translation matrix to be used by GTE

	for (u32 i = 0; i < NUM_FACES * 4; i += 4)
	{
		g_Quad = (POLY_G4*)g_NextPrim;
		setPolyG4(g_Quad);
		setRGB0(g_Quad, 255,   0, 255);
		setRGB1(g_Quad, 255, 255,   0);
		setRGB2(g_Quad,   0, 255, 255);
		setRGB3(g_Quad,   0, 255,   0);

		s32 OrderingTableZ, P, Flag, NormalClip;

		// load first 3 verts (GTE can only compute max. 3 vectors at a time)
		gte_ldv0(&g_CubeVertices[g_CubeFaces[i]]);
		gte_ldv1(&g_CubeVertices[g_CubeFaces[i + 1]]);
		gte_ldv2(&g_CubeVertices[g_CubeFaces[i + 2]]);

		// transform/project first 3 verts
		gte_rtpt();

		gte_nclip();
		gte_stopz(&NormalClip);

		if (NormalClip <= 0)
		{
			// Don't bother if the normal is facing away from us
			continue;
		}

		// Grab the transformed version of vert 0
		gte_stsxy0(&g_Quad->x0);

		// load the last vertex into the GTE
		gte_ldv0(&g_CubeVertices[g_CubeFaces[i + 3]]);

		// transform/project the last vertex
		gte_rtps();

		// grab transformed versions of vertices 1-3
		gte_stsxy3(&g_Quad->x1, &g_Quad->x2, &g_Quad->x3);

		gte_avsz4();
		gte_stotz(&OrderingTableZ);

		// if we've gotten a sensible Z value
		if (NormalClip > 0 && OrderingTableZ > 0 && OrderingTableZ < OT_LENGTH)
		{
			addPrim(g_OrderingTable[g_CurrentBuffer][OrderingTableZ], g_Quad);
			g_NextPrim += sizeof(POLY_G4);
		}
	}

	RotMatrix(&g_FloorRotation, &s_WorldMatrix);
	TransMatrix(&s_WorldMatrix, &g_FloorPosition);

	SetRotMatrix(&s_WorldMatrix);
	SetTransMatrix(&s_WorldMatrix);

	for (u32 i = 0; i < ArrayCount(g_FloorFaces); i += 3)
	{
		s32 OrderingTableZ, NormalClip;

		g_Triangle = (POLY_F3*)g_NextPrim;
		setPolyF3(g_Triangle);
		setRGB0(g_Triangle, 58, 53, 47); // grey

		gte_ldv0(&g_FloorVertices[g_FloorFaces[i]]);
		gte_ldv1(&g_FloorVertices[g_FloorFaces[i + 1]]);
		gte_ldv2(&g_FloorVertices[g_FloorFaces[i + 2]]);

		gte_rtpt();

		gte_nclip();
		gte_stopz(&NormalClip);

		if (NormalClip > 0)
		{
			gte_stsxy3(&g_Triangle->x0, &g_Triangle->x1, &g_Triangle->x2);
			gte_avsz3();
			gte_stotz(&OrderingTableZ);

			if (OrderingTableZ > 0 && OrderingTableZ < OT_LENGTH)
			{
				addPrim(g_OrderingTable[g_CurrentBuffer][OrderingTableZ], g_Triangle);
				g_NextPrim += sizeof(POLY_F3);
			}

		}

	}
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
