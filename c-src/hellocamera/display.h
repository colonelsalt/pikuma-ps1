#pragma once

#include <libgpu.h>
#include "util.h"

constexpr u32 VIDEO_MODE = 0;
constexpr u32 SCREEN_RES_X = 320;
constexpr u32 SCREEN_RES_Y = 240;
constexpr u32 SCREEN_CENTRE_X = SCREEN_RES_X >> 1;
constexpr u32 SCREEN_CENTRE_Y = SCREEN_RES_Y >> 1;
constexpr u32 SCREEN_Z = 400;

struct double_buffer
{
	// One man's draw buffer is another man's display buffer
	DRAWENV Draw[2];
	DISPENV Disp[2];
};

u16 GetCurrBuff();

void ScreenInit();

void DisplayFrame();

