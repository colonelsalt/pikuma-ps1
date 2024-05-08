#pragma once

#include <cstdint>

typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef float f32;
typedef double f64;
typedef uint32_t u32;
typedef int32_t s32;
typedef int32_t b32;
typedef int64_t s64;
typedef uint64_t u64;

#define ArrayCount(X) (sizeof(X) / sizeof((X)[0]))

#if _WIN32
#define Assert(X) {if (!(X)) __debugbreak();}
#else
#define Assert(X) {if (!(X)) *(int*)0 = 42;}
#endif

enum byte_order
{
	ByteOrder_LittleEndian,
	ByteOrder_BigEndian
};

u8* FileRead(char* FileName, u32* OutLength);

u16 GetU16(u8* ByteBuffer, u32* OutByteIndex, byte_order ByteOrder);
u32 GetU32(u8* ByteBuffer, u32* OutByteIndex, byte_order ByteOrder);