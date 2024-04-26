#include <libetc.h>
#include <libgpu.h>
#include <libgte.h>
#include <libcd.h>
#include <malloc.h>
#include <stdio.h>
#include "inline_n.h"

#include "util.h"
#include "joypad.h"
#include "globals.h"
#include "display.h"
#include "camera.h"

extern char __heap_start, __sp;

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

POLY_G4* g_Quad;

SVECTOR g_CubeRotation = { 0, 0, 0 };
VECTOR g_Scale = { ONE, ONE, ONE };

static MATRIX s_WorldMatrix;
static MATRIX s_ViewMatrix;

camera g_Camera;

VECTOR g_Velocity = { 0, 0, 0 };
VECTOR g_Acceleration = { 0, 0, 0 };
VECTOR g_CubePosition = { 0, 0, 0 };

void Setup()
{
	InitHeap3((u32 *)&__heap_start, (&__sp - 0x50'00) - &__heap_start);

	ScreenInit();

	CdInit();

	JoypadInit();

	ResetNextPrim(GetCurrBuff());

	g_Camera.Position = { 500, -1'000, -1'500 };
	g_Camera.LookAt = {};

	u32 FileSize;
	u8* FileBytes = FileRead("\\MODEL.BIN;1", &FileSize);
	printf("Read %u bytes from MODEL.BIN\n", FileSize);

	//free(FileBytes);
}

void Update()
{
	// Clear ordering table
	EmptyOt(GetCurrBuff());

	JoypadUpdate();

	if (JoypadCheck(PAD1_LEFT))
	{
		g_Camera.Position.vx -= 50;
	}
	if (JoypadCheck(PAD1_RIGHT))
	{
		g_Camera.Position.vx += 50;
	}
	if (JoypadCheck(PAD1_UP))
	{
		g_Camera.Position.vy -= 50;
	}
	if (JoypadCheck(PAD1_DOWN))
	{
		g_Camera.Position.vy += 50;
	}
	if (JoypadCheck(PAD1_CROSS))
	{
		g_Camera.Position.vz += 50;
	}
	if (JoypadCheck(PAD1_CIRCLE))
	{
		g_Camera.Position.vz -= 50;
	}

	g_Velocity.vx += g_Acceleration.vx;
	g_Velocity.vy += g_Acceleration.vy;
	g_Velocity.vz += g_Acceleration.vz;

	g_CubePosition.vx += g_Velocity.vx >> 1;
	g_CubePosition.vy += g_Velocity.vy >> 1;
	g_CubePosition.vz += g_Velocity.vz >> 1;

	if (g_CubePosition.vy > 400)
	{
		g_Velocity.vy *= -1;
	}

	VECTOR GlobalUp = { 0, -ONE, 0 };
	LookAt(&g_Camera, &g_Camera.Position, &g_CubePosition, &GlobalUp);

	RotMatrix(&g_CubeRotation, &s_WorldMatrix); // populate matrix with rotation values
	TransMatrix(&s_WorldMatrix, &g_CubePosition); // populate matrix with translation values
	ScaleMatrix(&s_WorldMatrix, &g_Scale); // populate matrix with scale values

	CompMatrixLV(&g_Camera.LookAt, &s_WorldMatrix, &s_ViewMatrix);

	SetRotMatrix(&s_ViewMatrix); // sets rot/scale matrix to be used by the GTE (for upcoming RotTransPers call)
	SetTransMatrix(&s_ViewMatrix); // sets translation matrix to be used by GTE

	for (u32 i = 0; i < NUM_FACES * 4; i += 4)
	{
		g_Quad = (POLY_G4*)GetNextPrim();
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
		if (OrderingTableZ > 0 && OrderingTableZ < OT_LENGTH)
		{
			addPrim(GetOtAt(GetCurrBuff(), OrderingTableZ), g_Quad);
			IncrementNextPrimitive(sizeof(POLY_G4));
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
