#pragma once

#include <libgte.h>
#include "util.h"

struct object
{
	SVECTOR Rotation;
	VECTOR Position;
	VECTOR Scale;

	u16 NumVerts;
	SVECTOR* Vertices;

	u16 NumFaces;
	u16* Faces;

	u16 NumColours;
	CVECTOR* Colours;
};