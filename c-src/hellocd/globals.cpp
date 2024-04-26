#include "globals.h"

#include <libgpu.h>

static u32 s_OrderingTable[2][OT_LENGTH];
static u8 s_PrimBuff[2][PRIMBUFF_LENGTH];
static u8* s_NextPrim;

void EmptyOt(u16 CurrBuffIndex)
{
	ClearOTagR(s_OrderingTable[CurrBuffIndex], OT_LENGTH);
}

void SetOtAt(u16 CurrBuff, u32 Index, u32 Value)
{
	addPrim(s_OrderingTable[CurrBuff][Index], Value);
	s_OrderingTable[CurrBuff][Index] = Value;
}

u32* GetOtAt(u16 CurrBuff, u32 Index)
{
	Assert(CurrBuff < 2 && Index < OT_LENGTH);
	return &s_OrderingTable[CurrBuff][Index];
}

void IncrementNextPrimitive(u32 Size)
{
	s_NextPrim += Size;
}

void SetNextPrim(u8* Value)
{
	// is this what we want..?
	s_NextPrim = Value;
}

void ResetNextPrim(u16 CurrBuffIndex)
{
	s_NextPrim = s_PrimBuff[CurrBuffIndex];
}

u8* GetNextPrim()
{
	return s_NextPrim;
}