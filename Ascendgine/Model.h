#pragma once

#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include "BinaryReaderX.h"

#include "RenderMaterial.h"
#include "Mesh.h"
#include "Effect.h"

class Model {
	public:
		RenderMaterial** Materials;
		Mesh** Meshes;

		int TotalMaterials;
		int TotalMeshes;

		Model(void);
		~Model(void);

		void LoadFromFile(char* filename);

		void RenderOpaque(Effect* fx, int passID);
};