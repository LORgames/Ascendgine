#pragma once

#include <GL/glew.h>
#include <iostream>
#include <fstream>

#include "RenderMaterial.h"

typedef struct {
	float Position[3];
	float Normals[3];
	float UVs[2];
} Vertex;

/*
const Vertex Vertices[] = {
    { { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
    // Top
    { { -0.2f, 0.8f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
    { { 0.2f, 0.8f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
    { { 0.0f, 0.8f, 0.0f }, { 0.0f, 1.0f, 1.0f } },
    { { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
    // Bottom
    { { -0.2f, -0.8f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
    { { 0.2f, -0.8f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
    { { 0.0f, -0.8f, 0.0f }, { 0.0f, 1.0f, 1.0f } },
    { { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
    // Left
    { { -0.8f, -0.2f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
    { { -0.8f, 0.2f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
    { { -0.8f, 0.0f, 0.0f }, { 0.0f, 1.0f, 1.0f } },
    { { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
    // Right
    { { 0.8f, -0.2f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
    { { 0.8f, 0.2f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
    { { 0.8f, 0.0f, 0.0f }, { 0.0f, 1.0f, 1.0f } },
    { { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } }
};

const GLubyte Indices[] = {
    // Top
    0, 1, 3,
    0, 3, 2,
    3, 1, 4,
    3, 4, 2,
    // Bottom
    0, 5, 7,
    0, 7, 6,
    7, 5, 8,
    7, 8, 6,
    // Left
    0, 9, 11,
    0, 11, 10,
    11, 9, 12,
    11, 12, 10,
    // Right
    0, 13, 15,
    0, 15, 14,
    15, 13, 16,
    15, 16, 14
};
*/

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
		
		void RenderOpaque();
};

