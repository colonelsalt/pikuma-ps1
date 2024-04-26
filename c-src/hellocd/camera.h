#pragma once

#include "globals.h"

struct camera
{
	VECTOR Position;
	SVECTOR Rotation;
	MATRIX LookAt;
};

void LookAt(camera* Camera, VECTOR* Eye, VECTOR* Target, VECTOR* GlobalUp);