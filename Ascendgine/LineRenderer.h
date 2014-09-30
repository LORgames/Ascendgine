#pragma once

#include <stdint.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

#include "Effect.h"

struct LineVertex
{
  float Position[3];
  uint8_t Colour[3];
};

class LineRenderer
{
public:
  LineRenderer(uint32_t maxLines = 1000, bool isStatic = false, Effect* differentEffect = nullptr);
  ~LineRenderer();

  bool AddLineToRender(float x0, float y0, float z0, float x1, float y1, float z1, uint32_t colour);
  bool AddLineToRender(glm::vec3 p0, glm::vec3 p1, uint32_t colour);
  void Render(glm::mat4 wvp);

private:
  //Secret functions
  void CreateBuffers();

  //Other magic
  Effect* lineEffect;

  //Variables
  int usedLines;
  int maxLines;

  //Texture info
  uint32_t texID;

  //Ascengine variables
  uint32_t vaoID;
  uint32_t bufferID;

  LineVertex* vertices;
  int totalVertices;

  bool isStatic;
  bool hasChanged;
};

