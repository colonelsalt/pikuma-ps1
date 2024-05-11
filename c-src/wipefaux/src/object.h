#pragma once

#include <libgte.h>
#include "util.h"
#include "camera.h"

enum prim_flags : u16
{
	PrimFlags_SingleSided  = 1,
	PrimFlags_ShipEngine   = 1 << 1,
	PrimFlags_Translucent  = 1 << 2
};

enum prim_type : u16
{
	PrimType_F3  = 1,
	PrimType_FT3 = 2,
	PrimType_F4  = 3,
	PrimType_FT4 = 4,
	PrimType_G3  = 5,
	PrimType_GT3 = 6,
	PrimType_G4  = 7,
	PrimType_GT4 = 8,

	// Don't think we'll use these types:
	PrimType_LF2 = 9,
	PrimType_TSPR = 10,
	PrimType_BSPR = 11,
	PrimType_LSF3 = 12,
	PrimType_LSFT3 = 13,
	PrimType_LSF4  = 14,
	PrimType_LSFT4 = 15,
	PrimType_LSG3  = 16,
	PrimType_LSGT3 = 17,
	PrimType_LSG4  = 18,
	PrimType_LSGT4 = 19,
	PrimType_Spline = 20,
	PrimType_InfiniteLight = 21,
	PrimType_PointLight = 22,
	PrimType_Spotlight = 23
};

struct f3
{
	u16 Type;
	u16 Flags;
	u16 FaceIndices[3];
	u16 Padding;
	CVECTOR Colour;
};

struct ft3
{
	u16 Type;
	u16 Flags;
	u16 FaceIndices[3];
	u16 Texture;
	u16 Clut;
	u16 TPage;
	
	u8 U0;
	u8 V0;
	u8 U1;
	u8 V1;
	u8 U2;
	u8 V2;

	u16 Padding;
	CVECTOR Colour;
};

struct f4
{
	u16 Type;
	u16 Flags;
	u16 FaceIndices[4];
	CVECTOR Colour;
};

struct ft4
{
	u16 Type;
	u16 Flags;
	u16 FaceIndices[4];
	u16 Texture;
	u16 Clut;
	u16 TPage;
	
	u8 U0;
	u8 V0;
	u8 U1;
	u8 V1;
	u8 U2;
	u8 V2;
	u8 U3;
	u8 V3;

	u16 Padding;
	CVECTOR Colour;
};

struct g3
{
	u16 Type;
	u16 Flags;
	u16 FaceIndices[3];
	u16 Padding;
	CVECTOR Colours[3];
};

struct gt3
{
	u16 Type;
	u16 Flags;
	u16 FaceIndices[3];
	u16 Texture;
	u16 Clut;
	u16 TPage;

	u8 U0;
	u8 V0;
	u8 U1;
	u8 V1;
	u8 U2;
	u8 V2;

	u16 Padding;
	CVECTOR Colours[3];
};

struct g4
{
	u16 Type;
	u16 Flags;
	u16 FaceIndices[4];
	u16 Padding;
	CVECTOR Colours[4];
};

struct gt4
{
	u16 Type;
	u16 Flags;
	u16 FaceIndices[4];
	u16 Texture;
	u16 Clut;
	u16 TPage;

	u8 U0;
	u8 V0;
	u8 U1;
	u8 V1;
	u8 U2;
	u8 V2;
	u8 U3;
	u8 V3;

	u16 Padding;
	CVECTOR Colours[4];
};

struct spr
{
	u16 Type;
	u16 Flags;
	u16 Index;
	u16 Width;
	u16 Height;
	u16 Texture;
	CVECTOR Colour;
};

struct prim_node
{
	u16 Type;
	u16 Flags;
	void* Data;
};

struct object
{
	u8 Name[16];
	
	u16 Flags;
	
	VECTOR OriginWorldSpace;

	u16 NumVertices;
	SVECTOR* Vertices;

	u16 NumNormals;
	SVECTOR* Normals;

	u16 NumPrimitives;
	prim_node* Primitives;

	SVECTOR Rotation;
	VECTOR Position;
	VECTOR Scale;
};

void LoadPrm(object* OutObject, char* FileName);
void RenderObject(object* Object, camera* Camera);

