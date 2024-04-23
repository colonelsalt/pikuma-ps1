#pragma once

#include "util.h"

constexpr u32 OT_LENGTH = 8'192;
constexpr u32 PRIMBUFF_LENGTH = 131'072;

void EmptyOt(u16 CurrBuffIndex);

void SetOtAt(u16 CurrBuff, u32 Index, u32 Value);
u32* GetOtAt(u16 CurrBuff, u32 Index);

void IncrementNextPrimitive(u32 Size);
void SetNextPrim(u8* Value);
void ResetNextPrim(u16 CurrBuffIndex);
u8* GetNextPrim();