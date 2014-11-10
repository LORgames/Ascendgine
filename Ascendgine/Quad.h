#pragma once
#include "Mesh.h"

Mesh* Create_Quad()
{
  Vertex* vertices = new Vertex[4];
  int* indices = new int[6];

  //1.
  vertices[0].Position[0] = -1.f; vertices[0].Position[1] = 1.f; vertices[0].Position[2] = 0.f;
  vertices[0].UVs[0] = 0; vertices[0].UVs[1] = 1;
  vertices[0].Normals[0] = vertices[0].Normals[1] = vertices[0].Normals[2] = 0;
  //2.
  vertices[1].Position[0] = -1.f; vertices[1].Position[1] = -1.f; vertices[1].Position[2] = 0.f;
  vertices[1].UVs[0] = 0; vertices[1].UVs[1] = 0;
  vertices[1].Normals[0] = vertices[1].Normals[1] = vertices[1].Normals[2] = 0;
  //3.
  vertices[2].Position[0] = 1.f; vertices[2].Position[1] = -1.f; vertices[2].Position[2] = 0.f;
  vertices[2].UVs[0] = 1; vertices[2].UVs[1] = 0;
  vertices[2].Normals[0] = vertices[2].Normals[1] = vertices[2].Normals[2] = 0;
  //4.
  vertices[3].Position[0] = 1.f; vertices[3].Position[1] = 1.f; vertices[3].Position[2] = 0.f;
  vertices[3].UVs[0] = 1; vertices[3].UVs[1] = 1;
  vertices[3].Normals[0] = vertices[3].Normals[1] = vertices[3].Normals[2] = 0;

  indices[0] = 0;
  indices[1] = 1;
  indices[2] = 2;

  indices[3] = 0;
  indices[4] = 2;
  indices[5] = 3;

  fprintf(stdout, "Creating quad.\n");

  Mesh* mesh;
  Mesh_Create(&mesh, nullptr, vertices, 4, indices, 6);
  return mesh;
}