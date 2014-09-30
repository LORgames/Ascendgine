#include "LineRenderer.h"

static Effect* g_LineRendererShader = nullptr;

LineRenderer::LineRenderer(uint32_t maxLines, bool isStatic, Effect* differentEffect)
{
  this->isStatic = isStatic;
  this->maxLines = maxLines;

  vertices = new LineVertex[maxLines * 2];
  totalVertices = maxLines * 2;
  usedLines = 0;

  CreateBuffers();

  if (differentEffect != nullptr)
  {
    lineEffect = differentEffect;
  }
  else
  {
    if (g_LineRendererShader == nullptr)
      g_LineRendererShader = new Effect("../shaders/Line.vs", "../shaders/Line.ps");

    lineEffect = g_LineRendererShader;
  }
}

LineRenderer::~LineRenderer()
{
  delete[] vertices;
}

bool LineRenderer::AddLineToRender(float x0, float y0, float z0, float x1, float y1, float z1, uint32_t colour)
{
  if (usedLines == maxLines)
    return false;

  vertices[usedLines * 2 + 0].Position[0] = x0;
  vertices[usedLines * 2 + 0].Position[1] = y0;
  vertices[usedLines * 2 + 0].Position[2] = z0;
  vertices[usedLines * 2 + 0].Colour[0] = ((colour >> 16) & 0xFF);
  vertices[usedLines * 2 + 0].Colour[1] = ((colour >> 8) & 0xFF);
  vertices[usedLines * 2 + 0].Colour[2] = (colour & 0xFF);

  vertices[usedLines * 2 + 1].Position[0] = x1;
  vertices[usedLines * 2 + 1].Position[1] = y1;
  vertices[usedLines * 2 + 1].Position[2] = z1;
  vertices[usedLines * 2 + 1].Colour[0] = ((colour >> 16) & 0xFF);
  vertices[usedLines * 2 + 1].Colour[1] = ((colour >> 8) & 0xFF);
  vertices[usedLines * 2 + 1].Colour[2] = (colour & 0xFF);

  hasChanged = true;

  usedLines++;
  return true;
}

void LineRenderer::Render(glm::mat4 wvp)
{
  Camera c = Camera();
  c.Projection = wvp;

  lineEffect->Apply(&c);

  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);

  glBindVertexArray(vaoID);
  glBindBuffer(GL_ARRAY_BUFFER, bufferID);

  if (!isStatic || hasChanged)
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(LineVertex) * 2 * usedLines, vertices);

  glDrawArrays(GL_LINES, 0, usedLines*2);

  if (!isStatic)
    usedLines = 0;
}

void LineRenderer::CreateBuffers()
{
  glGenVertexArrays(1, &vaoID);
  glBindVertexArray(vaoID);

  GLenum ErrorCheckValue = glGetError();
  const size_t BufferSize = sizeof(LineVertex) * totalVertices;
  const size_t VertexSize = sizeof(LineVertex);

  glGenBuffers(1, &bufferID);
  glBindBuffer(GL_ARRAY_BUFFER, bufferID);
  glBufferData(GL_ARRAY_BUFFER, BufferSize, vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)(0 * sizeof(float)));
  glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, VertexSize, (GLvoid*)(3 * sizeof(float)));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  ErrorCheckValue = glGetError();
  if (ErrorCheckValue != GL_NO_ERROR) {
    fprintf(stderr, "ERROR: Could not create a VBO: %s \n", gluErrorString(ErrorCheckValue));
    exit(-1);
  }
}