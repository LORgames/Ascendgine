#ifndef QUADRENDERER_H
#define QUADRENDERER_H 1

const int MAXIMUM_QUADS_PER_RENDERER = 1000;

#include <GL/glew.h>

struct Vertex
{
  float Position[2];
  float UVs[2];
};

class QuadRenderer
{
public:
  QuadRenderer(GLuint textureID, int texWidth, int texHeight, int overloadMaxQuads = 0);
  ~QuadRenderer();

  bool AddQuadToRender(float x, float y, float texX, float texY, float texW, float texH);
  void Render();

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
  Vertex* vertices;

  int totalIndices;
  int totalVertices;

  int maxQuads;
};

#endif