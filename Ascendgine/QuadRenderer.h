#ifndef QUADRENDERER_H
#define QUADRENDERER_H 1

const int MAXIMUM_QUADS_PER_RENDERER = 1000;

#include <GL/glew.h>
#include "Texture.h"

struct QuadVertex
{
  float Position[2];
  float UVs[2];
};

class QuadRenderer
{
public:
  QuadRenderer(Texture* texture, int overloadMaxQuads = 0);
  ~QuadRenderer();

  bool AddQuadToRender(float x, float y, float texX, float texY, float texW, float texH);
  void Render();

  static void Resized(int width, int height);

private:
  //Secret functions
  void CreateBuffers();

  //Variables
  int usedQuads;

  //Texture info
  float textureWidth;
  float textureHeight;
  GLuint texID;

  //Ascengine variables
  GLuint vaoID;
  GLuint bufferID;
  GLuint indexBufferID;

  int* indices;
  QuadVertex* vertices;

  int totalIndices;
  int totalVertices;

  int maxQuads;
};

#endif