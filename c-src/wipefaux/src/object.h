#pragma once

#include <libgte.h>
#include "util.h"

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
	//primitive* Primitives;

	SVECTOR Rotation;
	SVECTOR Position;
	VECTOR Scale;
};

void LoadPrm(object* OutObject, char* FileName);


