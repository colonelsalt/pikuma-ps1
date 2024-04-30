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
#include "object.h"

extern char __heap_start, __sp;

object g_Object;

POLY_FT4* g_Quad;

static MATRIX s_WorldMatrix;
static MATRIX s_ViewMatrix;

camera g_Camera;

struct tim_data
{
	u32 Mode;
	RECT Texture;
	RECT Clut;
};
tim_data g_Tim;


void LoadTexture(char* FileName, tim_data* OutTimData)
{
	u32 FileSize;
	u32 ByteIndex = 0;
	u32* FileBytes = (u32*)FileRead(FileName, &FileSize);
	if (!FileBytes)
	{
		return;
	}

	OpenTIM(FileBytes);
	TIM_IMAGE Tim;
	ReadTIM(&Tim);

	LoadImage(Tim.prect, Tim.paddr);
	DrawSync(0);

	// If TIM image has a clut, we need to copy that to VRAM as well
	if (Tim.mode & (1 << 3))
	{
		LoadImage(Tim.crect, Tim.caddr);
		DrawSync(0);
	}

	OutTimData->Mode = Tim.mode;
	OutTimData->Texture = *Tim.prect;
	OutTimData->Clut = *Tim.crect;

	free3(FileBytes);
}

void LoadModel(char* FileName, object* OutObject)
{
	u32 FileSize;
	u8* FileBytes = FileRead(FileName, &FileSize);
	u32 ByteIndex = 0;

	OutObject->NumVerts = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
	OutObject->Vertices = (SVECTOR*)malloc3(OutObject->NumVerts * sizeof(SVECTOR));
	for (u32 i = 0; i < OutObject->NumVerts; i++)
	{
		SVECTOR* Vertex = OutObject->Vertices + i;

		Vertex->vx = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
		Vertex->vy = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
		Vertex->vz = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);

		printf("Vertex %d: (%d, %d, %d)\n", i, Vertex->vx, Vertex->vy, Vertex->vz);
	}

	OutObject->NumFaces = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
	OutObject->Faces = (u16*)malloc3(OutObject->NumFaces * 4 * sizeof(u16));
	for (u32 i = 0; i < OutObject->NumFaces * 4; i += 4)
	{
		OutObject->Faces[i] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
		OutObject->Faces[i + 1] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
		OutObject->Faces[i + 2] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
		OutObject->Faces[i + 3] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
		printf("Face %u: (%u, %u, %u)\n", i / 4, OutObject->Faces[i], OutObject->Faces[i + 1], OutObject->Faces[i + 2], OutObject->Faces[i + 3]);
	}

	OutObject->NumColours = (u16)FileBytes[ByteIndex++];
	OutObject->Colours = (CVECTOR*)malloc3(OutObject->NumColours * sizeof(CVECTOR));
	for (u32 i = 0; i < OutObject->NumColours; i++)
	{
		CVECTOR* Colour = OutObject->Colours + i;
		Colour->r = FileBytes[ByteIndex++];
		Colour->g = FileBytes[ByteIndex++];
		Colour->b = FileBytes[ByteIndex++];
		ByteIndex++;

		printf("Colour %u: (%u, %u, %u)\n", i, Colour->r, Colour->g, Colour->b);
	}

	free3(FileBytes);
}

void Setup()
{
	InitHeap3((u32 *)&__heap_start, (&__sp - 0x50'00) - &__heap_start);

	ScreenInit();

	CdInit();

	JoypadInit();

	ResetNextPrim(GetCurrBuff());

	g_Camera.Position = { 500, -1'000, -1'500 };
	g_Camera.LookAt = {};

	g_Object.Position = {};
	g_Object.Rotation = {};
	g_Object.Scale = { ONE, ONE, ONE };

	LoadModel("\\MODEL.BIN;1", &g_Object);
	LoadTexture("\\BRICKS.TIM;1", &g_Tim);
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

	VECTOR GlobalUp = { 0, -ONE, 0 };
	LookAt(&g_Camera, &g_Camera.Position, &g_Object.Position, &GlobalUp);

	RotMatrix(&g_Object.Rotation, &s_WorldMatrix); // populate matrix with rotation values
	TransMatrix(&s_WorldMatrix, &g_Object.Position); // populate matrix with translation values
	ScaleMatrix(&s_WorldMatrix, &g_Object.Scale); // populate matrix with scale values

	CompMatrixLV(&g_Camera.LookAt, &s_WorldMatrix, &s_ViewMatrix);

	SetRotMatrix(&s_ViewMatrix); // sets rot/scale matrix to be used by the GTE (for upcoming RotTransPers call)
	SetTransMatrix(&s_ViewMatrix); // sets translation matrix to be used by GTE

	for (u32 i = 0; i < g_Object.NumFaces * 4; i += 4)
	{
		u32 QuadIndex = i / 4;
		g_Quad = (POLY_FT4*)GetNextPrim();
		setPolyFT4(g_Quad);

		CVECTOR* Colour = g_Object.Colours + QuadIndex;
		setRGB0(g_Quad, 128, 128, 128);

		g_Quad->u0 =  0;  g_Quad->v0 =  0;
		g_Quad->u1 = 63;  g_Quad->v1 =  0;
		g_Quad->u2 =  0;  g_Quad->v2 = 63;
		g_Quad->u3 = 63;  g_Quad->v3 = 63;

		g_Quad->tpage = getTPage(g_Tim.Mode & 0x3, 0, g_Tim.Texture.x, g_Tim.Texture.y);
		g_Quad->clut = getClut(g_Tim.Clut.x, g_Tim.Clut.y);

		s32 OrderingTableZ, P, Flag, NormalClip;

		// load first 3 verts (GTE can only compute max. 3 vectors at a time)
		gte_ldv0(&g_Object.Vertices[g_Object.Faces[i]]);
		gte_ldv1(&g_Object.Vertices[g_Object.Faces[i + 1]]);
		gte_ldv2(&g_Object.Vertices[g_Object.Faces[i + 2]]);

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
		gte_ldv0(&g_Object.Vertices[g_Object.Faces[i + 3]]);

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
			IncrementNextPrimitive(sizeof(POLY_FT4));
		}
	}
	g_Object.Rotation.vy += 20;
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
