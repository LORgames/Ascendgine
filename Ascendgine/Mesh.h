#pragma once

#include <iostream>
#include <fstream>
#include <cstring>

#include <GL/glew.h>

#include "RenderMaterial.h"
#include "Effect.h"

typedef struct {
	float Position[3];
	float Normals[3];
	float UVs[2];
} Vertex;

class Mesh {
	private:
		GLuint vaoID;
		GLuint bufferID;
		GLuint indexBufferID;

		int* indices;
		Vertex* vertices;

		int totalIndices;
		int totalVertices;

		RenderMaterial* Material;

	public:
		Mesh(RenderMaterial* _mat, Vertex* _verts, int totalVerts, int* _indices, int totalIndices);
		~Mesh(void);
		
		void RenderOpaque(Effect* fx);
};

