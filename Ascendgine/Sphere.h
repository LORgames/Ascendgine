#pragma once
#include "Mesh.h"

#define FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/fast_square_root.hpp>

#define SPHERE_VERT(x,y,z,q) memset(&vertices[q], 0, sizeof(Vertex)); vertices[q].Position[0] = x; vertices[q].Position[1] = y; vertices[q].Position[2] = z;
#define ADDTRIANGLE(x,y,z,s) indices[s*3] = x; indices[s*3+1] = y; indices[s*3+2] = z;

Mesh* Create_Sphere(int refinement = 0, RenderMaterial* _mat = nullptr)
{
  assert(refinement == 0);

  float t = (1.0f + glm::fastSqrt(5.0f)) / 2.0f;

  Vertex* vertices = new Vertex[12];
  int* indices = new int[20*3];
    
  SPHERE_VERT(-1, t, 0, 0);
  SPHERE_VERT(1, t, 0, 1);
  SPHERE_VERT(-1, -t, 0, 2);
  SPHERE_VERT(1, -t, 0, 3);

  SPHERE_VERT(0, -1, t, 4);
  SPHERE_VERT(0, 1, t, 5);
  SPHERE_VERT(0, -1, -t, 6);
  SPHERE_VERT(0, 1, -t, 7);

  SPHERE_VERT(t, 0, -1, 8);
  SPHERE_VERT(t, 0, 1, 9);
  SPHERE_VERT(-t, 0, -1, 10);
  SPHERE_VERT(-t, 0, 1, 11);

  ADDTRIANGLE(0, 11, 5, 0);
  ADDTRIANGLE(0, 5, 1, 1);
  ADDTRIANGLE(0, 1, 7, 2);
  ADDTRIANGLE(0, 7, 10, 3);
  ADDTRIANGLE(0, 10, 11, 4);

  // 5 adjacent faces
  ADDTRIANGLE(1, 5, 9, 5);
  ADDTRIANGLE(5, 11, 4, 6);
  ADDTRIANGLE(11, 10, 2, 7);
  ADDTRIANGLE(10, 7, 6, 8);
  ADDTRIANGLE(7, 1, 8, 9);

  // 5 faces around point 3
  ADDTRIANGLE(3, 9, 4, 10);
  ADDTRIANGLE(3, 4, 2, 11);
  ADDTRIANGLE(3, 2, 6, 12);
  ADDTRIANGLE(3, 6, 8, 13);
  ADDTRIANGLE(3, 8, 9, 14);

  // 5 adjacent faces
  ADDTRIANGLE(4, 9, 5, 15);
  ADDTRIANGLE(2, 4, 11, 16);
  ADDTRIANGLE(6, 2, 10, 17);
  ADDTRIANGLE(8, 6, 7, 18);
  ADDTRIANGLE(9, 8, 1, 19);

  Mesh* mesh;
  Mesh_Create(&mesh, _mat, vertices, 12, indices, 20 * 3);
  return mesh;
}
