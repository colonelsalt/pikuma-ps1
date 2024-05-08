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

static MATRIX s_WorldMatrix;
static MATRIX s_ViewMatrix;

camera g_Camera;

void Setup()
{
	InitHeap3((u32 *)&__heap_start, (&__sp - 0x50'00) - &__heap_start);

	ScreenInit();

	CdInit();

	JoypadInit();

	ResetNextPrim(GetCurrBuff());

	g_Camera.Position = { 500, -1'000, -1'500 };
	g_Camera.LookAt = {};

	LoadPrm(&g_Object, "\\ALLSH.PRM;1");
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
