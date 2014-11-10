#pragma once

#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include "BinaryReaderX.h"

#include "RenderMaterial.h"
#include "Mesh.h"
#include "Effect.h"

struct Model
{
		RenderMaterial** Materials;
		Mesh** Meshes;

		int TotalMaterials;
		int TotalMeshes;
};

void Model_Create(Model **ppModel);
void Model_Destroy(Model* pModel);

void Model_LoadFromFile(Model **ppModel, char* filename);
void Model_RenderOpaque(Model* pModel, int passID);