#include "util.h"

#include <libcd.h>
#include <stdio.h>
#include <malloc.h>

u8* FileRead(char* FileName, u32* OutLength)
{
	u8* Result = nullptr;
	CdlFILE FileHandle;
	if (CdSearchFile(&FileHandle, FileName))
	{
		printf("Found file %s in CD\n", FileName);
		s32 NumSectors = (FileHandle.size + 2'047) / 2'048;
		Result = (u8*)malloc3(NumSectors * 2'048);
		if (Result)
		{
			CdControl(CdlSetloc, (u8*)&FileHandle.pos, nullptr); // set read target to this file in the CD
			CdRead(NumSectors, (u32*)Result, CdlModeSpeed); // actually start reading (async)
			CdReadSync(0, nullptr); // wait for current read to finish
			*OutLength = FileHandle.size;
		}
		else
		{
			printf("ERROR: Failed to allocate %d sectors.\n", NumSectors);
		}
	}
	else
	{
		printf("File %s not found in the CD\n", FileName);
	}
	return Result;
}