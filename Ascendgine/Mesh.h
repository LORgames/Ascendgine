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
	protected:
		GLuint vaoID;
		GLuint bufferID;
		GLuint indexBufferID;

		int* indices;
		Vertex* vertices;

		int totalIndices;
		int totalVertices;

		RenderMaterial* Material;

		bool canRender;
	public:
		Mesh(RenderMaterial* _mat);
		Mesh(RenderMaterial* _mat, Vertex* _verts, int totalVerts, int* _indices, int totalIndices);
		~Mesh(void);

		void CreateMesh(Vertex* _verts, int _totalVerts, int* _indices, int _totalIndices);
		
		void RenderOpaque(Effect* fx);
};

