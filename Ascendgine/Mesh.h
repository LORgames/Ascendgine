#pragma once

#include <iostream>
#include <fstream>
#include <cstring>

#include <GL/glew.h>

#include "RenderMaterial.h"
#include "Effect.h"

typedef struct
{
	float Position[3];
	float Normals[3];
	float UVs[2];
} Vertex;

struct Mesh
{
  GLuint vaoID;
  GLuint bufferID;
  GLuint indexBufferID;

  int* indices;
  Vertex* vertices;

  int totalIndices;
  int totalVertices;

  Effect fx;
  RenderMaterial* Material;
};

void Mesh_Create(Mesh** ppMesh, RenderMaterial* _mat, Vertex* _verts, int totalVerts, int* _indices, int totalIndices);
void Mesh_Destroy(Mesh* mesh);

static Effect* Mesh_GetEffect(Mesh* mesh){ return &mesh->fx; }
static void Mesh_SetEffect(Mesh* mesh, Effect* newEffect) { mesh->fx = *newEffect; }

void Mesh_RenderOpaque(Mesh* mesh);
