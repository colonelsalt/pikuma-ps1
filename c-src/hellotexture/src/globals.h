#pragma once

#include "util.h"

#include <libgte.h>

constexpr u32 OT_LENGTH = 8'192;
constexpr u32 PRIMBUFF_LENGTH = 131'072;

void EmptyOt(u16 CurrBuffIndex);

void SetOtAt(u16 CurrBuff, u32 Index, u32 Value);
u32* GetOtAt(u16 CurrBuff, u32 Index);

void IncrementNextPrimitive(u32 Size);
void SetNextPrim(u8* Value);
void ResetNextPrim(u16 CurrBuffIndex);
u8* GetNextPrim();

inline VECTOR operator-(VECTOR& A, VECTOR& B)
{
	VECTOR Result = {};
	Result.vx = A.vx - B.vx;
	Result.vy = A.vy - B.vy;
	Result.vz = A.vz - B.vz;
	return Result;
}

inline VECTOR operator+(VECTOR& A, VECTOR& B)
{
	VECTOR Result;
	Result.vx = A.vx + B.vx;
	Result.vy = A.vy + B.vy;
	Result.vz = A.vz + B.vz;
	return Result;
}

inline VECTOR operator*(VECTOR& A, s32 B)
{
	VECTOR Result;
	Result.vx = A.vx * B;
	Result.vy = A.vy * B;
	Result.vz = A.vz * B;
	return Result;
}

inline VECTOR operator-(VECTOR& A)
{
	VECTOR Result = A * -1;
	return Result;
}