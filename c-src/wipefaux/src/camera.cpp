#include "camera.h"

void VectorCross(VECTOR* A, VECTOR* B, VECTOR* Out)
{
	OuterProduct12(A, B, Out);
}

void LookAt(camera* Camera, VECTOR* Eye, VECTOR* Target, VECTOR* GlobalUp)
{
	VECTOR Forward = *Target - *Eye;
	VECTOR ForwardNorm;
	VectorNormal(&Forward, &ForwardNorm);

	VECTOR Right;
	VectorCross(&Forward, GlobalUp, &Right); // how do I know which order to do the cross product in here??
	VECTOR RightNorm;
	VectorNormal(&Right, &RightNorm);

	VECTOR Up;
	VectorCross(&Forward, &Right, &Up);
	VECTOR UpNorm;
	VectorNormal(&Up, &UpNorm);

	Camera->LookAt.m[0][0] = RightNorm.vx;   Camera->LookAt.m[0][1] = RightNorm.vy;   Camera->LookAt.m[0][2] = RightNorm.vz;
	Camera->LookAt.m[1][0] = UpNorm.vx;      Camera->LookAt.m[1][1] = UpNorm.vy;      Camera->LookAt.m[1][2] = UpNorm.vz;
	Camera->LookAt.m[2][0] = ForwardNorm.vx; Camera->LookAt.m[2][1] = ForwardNorm.vy; Camera->LookAt.m[2][2] = ForwardNorm.vz;

	VECTOR TranslationOffset = -(*Eye);

	VECTOR Translation;
	ApplyMatrixLV(&Camera->LookAt, &TranslationOffset, &Translation);
	TransMatrix(&Camera->LookAt, &Translation);
}