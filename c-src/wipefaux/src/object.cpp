#include "object.h"
#include <stdio.h>
#include <malloc.h>
#include <libgpu.h>
#include "inline_n.h"

#include "display.h"

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

	OutObject->Primitives = (prim_node*)malloc3(OutObject->NumPrimitives * sizeof(prim_node));
	for (u32 i = 0; i < OutObject->NumPrimitives; i++)
	{
		prim_node* Primitive = OutObject->Primitives + i;
		Primitive->Type = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
		Primitive->Flags = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
		switch (Primitive->Type)
		{
			case PrimType_F3:
			{
				printf("Loaded flat-shaded triangle (F3).\n");
				f3* Triangle = (f3*)malloc3(sizeof(f3));
				
				Triangle->Type = PrimType_F3;
				Triangle->FaceIndices[0] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Triangle->FaceIndices[1] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Triangle->FaceIndices[2] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Triangle->Padding = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Triangle->Colour = GetCVector(FileBytes, &ByteIndex);

				Primitive->Data = Triangle;
			} break;
			
			case PrimType_FT3:
			{
				printf("Loaded flat-shaded textured triangle (FT3).\n");
				ft3* Triangle = (ft3*)malloc3(sizeof(ft3));

				Triangle->Type = PrimType_FT3;
				Triangle->FaceIndices[0] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Triangle->FaceIndices[1] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Triangle->FaceIndices[2] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Triangle->Texture = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Triangle->Clut = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Triangle->TPage = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Triangle->U0 = FileBytes[ByteIndex++];
				Triangle->V0 = FileBytes[ByteIndex++];
				Triangle->U1 = FileBytes[ByteIndex++];
				Triangle->V1 = FileBytes[ByteIndex++];
				Triangle->U2 = FileBytes[ByteIndex++];
				Triangle->V2 = FileBytes[ByteIndex++];
				Triangle->Padding = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Triangle->Colour = GetCVector(FileBytes, &ByteIndex);

				Primitive->Data = Triangle;
			} break;

			case PrimType_F4:
			{
				printf("Loaded flat-shaded quad (F4).\n");
				f4* Quad = (f4*)malloc3(sizeof(f4));

				Quad->Type = PrimType_F4;
				Quad->FaceIndices[0] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->FaceIndices[1] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->FaceIndices[2] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->FaceIndices[3] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->Colour = GetCVector(FileBytes, &ByteIndex);

				Primitive->Data = Quad;
			} break;

			case PrimType_FT4:
			{
				printf("Loaded flat-shaded textured quad (FT4).\n");
				ft4* Quad = (ft4*)malloc3(sizeof(ft4));

				Quad->Type = PrimType_FT4;
				Quad->FaceIndices[0] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->FaceIndices[1] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->FaceIndices[2] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->FaceIndices[3] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->Texture = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->Clut = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->TPage = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->U0 = FileBytes[ByteIndex++];
				Quad->V0 = FileBytes[ByteIndex++];
				Quad->U1 = FileBytes[ByteIndex++];
				Quad->V1 = FileBytes[ByteIndex++];
				Quad->U2 = FileBytes[ByteIndex++];
				Quad->V2 = FileBytes[ByteIndex++];
				Quad->U3 = FileBytes[ByteIndex++];
				Quad->V3 = FileBytes[ByteIndex++];
				Quad->Padding = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->Colour = GetCVector(FileBytes, &ByteIndex);

				Primitive->Data = Quad;
			} break;

			case PrimType_G3:
			{
				printf("Loaded Gourad-shaded triangle (G3).\n");
				g3* Triangle = (g3*)malloc3(sizeof(g3));

				Triangle->Type = PrimType_G3;
				Triangle->FaceIndices[0] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Triangle->FaceIndices[1] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Triangle->FaceIndices[2] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Triangle->Padding = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Triangle->Colours[0] = GetCVector(FileBytes, &ByteIndex);
				Triangle->Colours[1] = GetCVector(FileBytes, &ByteIndex);
				Triangle->Colours[2] = GetCVector(FileBytes, &ByteIndex);

				Primitive->Data = Triangle;
			} break;

			case PrimType_GT3:
			{
				printf("Loaded Gourad-shaded textured triangle (GT3).\n");
				gt3* Triangle = (gt3*)malloc3(sizeof(gt3));

				Triangle->Type = PrimType_GT3;
				Triangle->FaceIndices[0] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Triangle->FaceIndices[1] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Triangle->FaceIndices[2] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Triangle->Texture = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Triangle->Clut = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Triangle->TPage = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Triangle->U0 = FileBytes[ByteIndex++];
				Triangle->V0 = FileBytes[ByteIndex++];
				Triangle->U1 = FileBytes[ByteIndex++];
				Triangle->V1 = FileBytes[ByteIndex++];
				Triangle->U2 = FileBytes[ByteIndex++];
				Triangle->V2 = FileBytes[ByteIndex++];
				Triangle->Padding = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Triangle->Colours[0] = GetCVector(FileBytes, &ByteIndex);
				Triangle->Colours[1] = GetCVector(FileBytes, &ByteIndex);
				Triangle->Colours[2] = GetCVector(FileBytes, &ByteIndex);

				Primitive->Data = Triangle;
			} break;

			case PrimType_G4:
			{
				printf("Loaded Gourad-shaded quad (G4).\n");
				g4* Quad = (g4*)malloc3(sizeof(g4));

				Quad->Type = PrimType_G4;
				Quad->FaceIndices[0] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->FaceIndices[1] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->FaceIndices[2] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->FaceIndices[3] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->Padding = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->Colours[0] = GetCVector(FileBytes, &ByteIndex);
				Quad->Colours[1] = GetCVector(FileBytes, &ByteIndex);
				Quad->Colours[2] = GetCVector(FileBytes, &ByteIndex);
				Quad->Colours[3] = GetCVector(FileBytes, &ByteIndex);

				Primitive->Data = Quad;
			} break;

			case PrimType_GT4:
			{
				printf("Loaded Gourad-shaded textured quad (GT4).\n");
				gt4* Quad = (gt4*)malloc3(sizeof(gt4));

				Quad->Type = PrimType_GT4;
				Quad->FaceIndices[0] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->FaceIndices[1] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->FaceIndices[2] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->FaceIndices[3] = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->Texture = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->Clut = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->TPage = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->U0 = FileBytes[ByteIndex++];
				Quad->V0 = FileBytes[ByteIndex++];
				Quad->U1 = FileBytes[ByteIndex++];
				Quad->V1 = FileBytes[ByteIndex++];
				Quad->U2 = FileBytes[ByteIndex++];
				Quad->V2 = FileBytes[ByteIndex++];
				Quad->U3 = FileBytes[ByteIndex++];
				Quad->V3 = FileBytes[ByteIndex++];
				Quad->Padding = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Quad->Colours[0] = GetCVector(FileBytes, &ByteIndex);
				Quad->Colours[1] = GetCVector(FileBytes, &ByteIndex);
				Quad->Colours[2] = GetCVector(FileBytes, &ByteIndex);
				Quad->Colours[3] = GetCVector(FileBytes, &ByteIndex);

				Primitive->Data = Quad;
			} break;

			case PrimType_TSPR:
			case PrimType_BSPR: // sprite types?? Not really sure what we're doing here
			{
				printf("Loaded a sprite..?\n");

				spr* Sprite = (spr*)malloc3(sizeof(spr));

				Sprite->Type = PrimType_TSPR;
				Sprite->Index = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Sprite->Width = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Sprite->Height = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Sprite->Texture = GetU16(FileBytes, &ByteIndex, ByteOrder_BigEndian);
				Sprite->Colour = GetCVector(FileBytes, &ByteIndex);

				Primitive->Data = Sprite;
			} break;

			case PrimType_Spline:
			{
				ByteIndex += 52;
			} break;

			case PrimType_PointLight:
			{
				ByteIndex += 24;
			} break;

			case PrimType_Spotlight:
			{
				ByteIndex += 36;
			} break;

			case PrimType_InfiniteLight:
			{
				ByteIndex += 12;
			} break;

			case PrimType_LSF3:
			{
				ByteIndex += 12;
			} break;

			case PrimType_LSFT3:
			{
				ByteIndex += 24;
			} break;

			case PrimType_LSF4:
			{
				ByteIndex += 16;
			} break;

			case PrimType_LSFT4:
			{
				ByteIndex += 28;
			} break;

			case PrimType_LSG3:
			{
				ByteIndex += 24;
			} break;

			case PrimType_LSGT3:
			{
				ByteIndex += 36;
			} break;

			case PrimType_LSG4:
			{
				ByteIndex += 32;
			} break;

			case PrimType_LSGT4:
			{
				ByteIndex += 42;
			} break;
		}
	}

	OutObject->Position = OutObject->OriginWorldSpace;
	OutObject->Scale = { ONE, ONE, ONE };
	OutObject->Rotation = {};

	free3(FileBytes);
}

void RenderObject(object* Object, camera* Camera)
{
	MATRIX ModelMat;

	RotMatrix(&Object->Rotation, &ModelMat);
	TransMatrix(&ModelMat, &Object->Position);
	ScaleMatrix(&ModelMat, &Object->Scale);

	MATRIX ModelView;
	CompMatrixLV(&Camera->LookAt, &ModelMat, &ModelView);

	SetRotMatrix(&ModelView);
	SetTransMatrix(&ModelView);

	for (u32 i = 0; i < Object->NumPrimitives; i++)
	{
		prim_node* PrimNode = Object->Primitives + i;
		switch (PrimNode->Type)
		{
			case PrimType_F3:
			case PrimType_FT3:
			case PrimType_G3:
			case PrimType_GT3:
			{
				// Render all these as flat-shaded triangles for now
				f3* Triangle = (f3*)PrimNode->Data;
				POLY_F3* OtPrim = (POLY_F3*)GetNextPrim();
				
				// Load vertices into GTE
				gte_ldv0(Object->Vertices + Triangle->FaceIndices[0]);
				gte_ldv1(Object->Vertices + Triangle->FaceIndices[1]);
				gte_ldv2(Object->Vertices + Triangle->FaceIndices[2]);
				gte_rtpt(); // project vertices

				// Verify that normals aren't facing away from us
				s32 NClip;
				gte_nclip();
				gte_stopz(&NClip);
				if (NClip < 0)
				{
					continue;
				}
				// Load back projected vertices
				gte_stsxy3(&OtPrim->x0, &OtPrim->x1, &OtPrim->x2);
				
				s32 OrderingTableZ;
				gte_avsz3(); // get the average of the z-values of the projected vertices
				gte_stotz(&OrderingTableZ);
				if (OrderingTableZ > 0 && OrderingTableZ < OT_LENGTH)
				{
					SetPolyF3(OtPrim);
					
					// yellow
					OtPrim->r0 = 255;
					OtPrim->g0 = 255;
					OtPrim->b0 = 0;

					addPrim(GetOtAt(GetCurrBuff(), OrderingTableZ), OtPrim);
					IncrementNextPrimitive(sizeof(POLY_F3));
				}
			} break;

			case PrimType_F4:
			case PrimType_FT4:
			case PrimType_G4:
			case PrimType_GT4:
			{
				f4* Quad = (f4*)PrimNode->Data;
				POLY_F4* OtPrim = (POLY_F4*)GetNextPrim();

				// Send first 3 verts to GTE
				gte_ldv0(Object->Vertices + Quad->FaceIndices[0]);
				gte_ldv1(Object->Vertices + Quad->FaceIndices[1]);
				gte_ldv2(Object->Vertices + Quad->FaceIndices[2]);
				gte_rtpt();

				s32 NClip;
				gte_nclip();
				gte_stopz(&NClip);
				if (NClip < 0)
				{
					continue;
				}

				gte_stsxy0(&OtPrim->x0); // get back first projected vertex
				gte_ldv0(Object->Vertices + Quad->FaceIndices[3]);
				gte_rtps(); // transform final vertex
				gte_stsxy3(&OtPrim->x1, &OtPrim->x2, &OtPrim->x3);
				gte_avsz4();

				s32 OrderingTableZ;
				gte_stotz(&OrderingTableZ);
				if (OrderingTableZ > 0 && OrderingTableZ < OT_LENGTH)
				{
					SetPolyF4(OtPrim);

					// Magenta
					OtPrim->r0 = 255;
					OtPrim->g0 = 0;
					OtPrim->b0 = 255;
					addPrim(GetOtAt(GetCurrBuff(), OrderingTableZ), OtPrim);
					IncrementNextPrimitive(sizeof(POLY_F4));
				}
			} break;
		}
	}
}