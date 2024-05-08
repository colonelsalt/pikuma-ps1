#include "object.h"
#include <stdio.h>
#include <malloc.h>

void LoadPrm(object* OutObject, char* FileName)
{
	u32 FileSize;
	u8* FileBytes = FileRead(FileName, &FileSize);
	if (!FileBytes)
	{
		return;
	}
	u32 ByteIndex = 0;

	for (u32 i = 0; i < 16; i++)
	{
		OutObject->Name[i] = FileBytes[ByteIndex++];
	}
	printf("Loading object: %s\n", OutObject->Name);

	OutObject->NumVertices = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
	OutObject->Vertices = nullptr;
	ByteIndex += 6;
	printf("Found %u vertices\n", OutObject->NumVertices);

	OutObject->NumNormals = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
	OutObject->Normals = nullptr;
	ByteIndex += 6;
	printf("Found %u normals\n", OutObject->NumNormals);

	OutObject->NumPrimitives = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
	ByteIndex += 22;
	printf("Found %u 'primitives'\n", OutObject->NumPrimitives);

	OutObject->Flags = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
	ByteIndex += 26;

	OutObject->OriginWorldSpace.vx = GetU32(FileBytes, &ByteIndex, ByteOrder_BigEndian);
	OutObject->OriginWorldSpace.vy = GetU32(FileBytes, &ByteIndex, ByteOrder_BigEndian);
	OutObject->OriginWorldSpace.vz = GetU32(FileBytes, &ByteIndex, ByteOrder_BigEndian);
	ByteIndex += 48;

	printf("Thingy is at (%ld, %ld, %ld)\n", OutObject->OriginWorldSpace.vx, OutObject->OriginWorldSpace.vy, OutObject->OriginWorldSpace.vz);

	OutObject->Vertices = (SVECTOR*)malloc3(OutObject->NumVertices * sizeof(SVECTOR));
	for (u32 i = 0; i < OutObject->NumVertices; i++)
	{
		SVECTOR* Vertex = OutObject->Vertices + i;
		Vertex->vx = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
		Vertex->vy = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
		Vertex->vz = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
		ByteIndex += 2;

		printf("Vertex %u: (%d, %d, %d)\n", i, Vertex->vx, Vertex->vy, Vertex->vz);
	}

	printf("Read all the vertices\n");

	OutObject->Normals = (SVECTOR*)malloc3(OutObject->NumNormals * sizeof(SVECTOR));
	for (u32 i = 0; i < OutObject->NumNormals; i++)
	{
		SVECTOR* Normal = OutObject->Normals + i;
		Normal->vx = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
		Normal->vy = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
		Normal->vz = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
		ByteIndex += 2;

		printf("Normal %u: (%d, %d, %d)\n", i, Normal->vx, Normal->vy, Normal->vz);
	}
	printf("Read all the normals\n");
}