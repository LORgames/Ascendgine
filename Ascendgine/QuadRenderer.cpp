#include "QuadRenderer.h"

#include <string>

QuadRenderer::QuadRenderer(GLuint textureID, int texWidth, int texHeight, int overloadMaxQuads)
{
  if (overloadMaxQuads <= 0)
    maxQuads = MAXIMUM_QUADS_PER_RENDERER;
  else
    maxQuads = overloadMaxQuads;

  vertices = new Vertex[maxQuads * 4];
  indices = new int[maxQuads * 6];

  texID = textureID;
  textureWidth = (float)texWidth;
  textureHeight = (float)texHeight;

  //Create the VBO's
  vertices = vertices;
  totalVertices = maxQuads*4;
  indices = indices;
  totalIndices = maxQuads*6;

  CreateBuffers();
}

QuadRenderer::~QuadRenderer()
{
  delete[] vertices;
  delete[] indices;
}

bool QuadRenderer::AddQuadToRender(float x, float y, float texX, float texY, float w, float h)
{
  if (usedQuads == maxQuads)
    return false;

  vertices[usedQuads * 4 + 0].Position[0] = x;
  vertices[usedQuads * 4 + 0].Position[1] = y + h;
  vertices[usedQuads * 4 + 1].Position[0] = x;
  vertices[usedQuads * 4 + 1].Position[1] = y;
  vertices[usedQuads * 4 + 2].Position[0] = x + w;
  vertices[usedQuads * 4 + 2].Position[1] = y;
  vertices[usedQuads * 4 + 3].Position[0] = x + w;
  vertices[usedQuads * 4 + 3].Position[1] = y + h;

  vertices[usedQuads * 4 + 0].UVs[0] = (texX + 0) / textureWidth;
  vertices[usedQuads * 4 + 0].UVs[1] = (texY + h) / textureHeight;
  vertices[usedQuads * 4 + 1].UVs[0] = (texX + 0) / textureWidth;
  vertices[usedQuads * 4 + 1].UVs[1] = (texY + 0) / textureHeight;
  vertices[usedQuads * 4 + 2].UVs[0] = (texX + w) / textureWidth;
  vertices[usedQuads * 4 + 2].UVs[1] = (texY + 0) / textureHeight;
  vertices[usedQuads * 4 + 3].UVs[0] = (texX + w) / textureWidth;
  vertices[usedQuads * 4 + 3].UVs[1] = (texY + h) / textureHeight;

  indices[usedQuads * 6 + 0] = usedQuads * 4 + 0;
  indices[usedQuads * 6 + 1] = usedQuads * 4 + 1;
  indices[usedQuads * 6 + 2] = usedQuads * 4 + 2;
  indices[usedQuads * 6 + 3] = usedQuads * 4 + 0;
  indices[usedQuads * 6 + 4] = usedQuads * 4 + 2;
  indices[usedQuads * 6 + 5] = usedQuads * 4 + 3;

  usedQuads++;
  return true;
}

void QuadRenderer::Render()
{
  glBindVertexArray(vaoID);
  glBindBuffer(GL_ARRAY_BUFFER, bufferID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

  glBufferSubData(GL_ARRAY_BUFFER,0, sizeof(Vertex)* 4 * usedQuads, vertices);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(int)*6*usedQuads, indices);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texID);

  // Draw the triangles!
  glDrawElements(GL_TRIANGLES, usedQuads*6, GL_UNSIGNED_INT, 0);

  usedQuads = 0;
}

void QuadRenderer::CreateBuffers()
{
  glGenVertexArrays(1, &vaoID);
  glBindVertexArray(vaoID);

  GLenum ErrorCheckValue = glGetError();
  const size_t BufferSize = sizeof(Vertex)*totalVertices;
  const size_t VertexSize = sizeof(Vertex);

  glGenBuffers(1, &bufferID);
  glBindBuffer(GL_ARRAY_BUFFER, bufferID);
  glBufferData(GL_ARRAY_BUFFER, BufferSize, vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)(0 * sizeof(float)));
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)(2 * sizeof(float)));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glGenBuffers(1, &indexBufferID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*totalIndices, indices, GL_STATIC_DRAW);

  ErrorCheckValue = glGetError();
  if (ErrorCheckValue != GL_NO_ERROR) {
    fprintf(stderr, "ERROR: Could not create a VBO: %s \n", gluErrorString(ErrorCheckValue));
    exit(-1);
  }
}