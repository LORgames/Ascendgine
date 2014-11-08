#include "LineRenderer.h"
#include <GL/glew.h>

struct LineVertex
{
  float Position[3];
  uint8_t Colour[3];
};

struct LineRenderer
{
  //Other magic
  Effect* lineEffect;

  //Variables
  int32_t usedLines;
  int32_t maxLines;

  //Texture info
  uint32_t texID;

  //Ascengine variables
  uint32_t vaoID;
  uint32_t bufferID;

  LineVertex* vertices;
  int32_t totalVertices;

  bool isStatic;
  bool hasChanged;
};

static Effect g_LineRendererShader = { 0 };

void LR_CreateBuffers(LineRenderer* lr);

void LineRenderer_Create(LineRenderer** lr, uint32_t maxLines, bool isStatic, Effect* differentEffect)
{
  *lr = new LineRenderer;
  (*lr)->isStatic = isStatic;
  (*lr)->maxLines = maxLines;

  (*lr)->vertices = new LineVertex[maxLines * 2];
  (*lr)->totalVertices = maxLines * 2;
  (*lr)->usedLines = 0;

  LR_CreateBuffers(*lr);

  if (differentEffect != nullptr)
  {
    (*lr)->lineEffect = differentEffect;
  }
  else
  {
    if (g_LineRendererShader.id == 0)
    {
      Effect_CreateFromText(&g_LineRendererShader,
        "#version 330 core\n\n//Input Format\nlayout(location = 0) in vec3 vertexPosition_modelspace;\nlayout(location = 1) in vec3 vertexColours;\n\n//Ouput Format\nout vec3 ex_Colour;\n\n//Uniforms\nuniform mat4 Projection;\n\nvoid main()\n{\n  gl_Position = Projection * vec4(vertexPosition_modelspace, 1.0);\n  ex_Colour = vertexColours;\n}",
        "#version 330 core\n\n//Input Format\nin vec3 ex_Colour;\n\n//Output Format\nout vec4 out_Colour;\n\nvoid main()\n{\n  out_Colour = vec4(ex_Colour, 1);\n}");
    }

    (*lr)->lineEffect = &g_LineRendererShader;
  }
}

void LineRenderer_Destroy(LineRenderer* lr)
{
  delete[] lr->vertices;
  delete lr;
}

bool LineRenderer_AddLine(LineRenderer* lr, const float &x0, const float &y0, const float &z0, const float &x1, const float &y1, const float &z1, const uint32_t &colour)
{
  if (lr->usedLines == lr->maxLines)
    return false;

  lr->vertices[lr->usedLines * 2 + 0].Position[0] = x0;
  lr->vertices[lr->usedLines * 2 + 0].Position[1] = y0;
  lr->vertices[lr->usedLines * 2 + 0].Position[2] = z0;
  lr->vertices[lr->usedLines * 2 + 0].Colour[0] = ((colour >> 16) & 0xFF);
  lr->vertices[lr->usedLines * 2 + 0].Colour[1] = ((colour >> 8) & 0xFF);
  lr->vertices[lr->usedLines * 2 + 0].Colour[2] = (colour & 0xFF);

  lr->vertices[lr->usedLines * 2 + 1].Position[0] = x1;
  lr->vertices[lr->usedLines * 2 + 1].Position[1] = y1;
  lr->vertices[lr->usedLines * 2 + 1].Position[2] = z1;
  lr->vertices[lr->usedLines * 2 + 1].Colour[0] = ((colour >> 16) & 0xFF);
  lr->vertices[lr->usedLines * 2 + 1].Colour[1] = ((colour >> 8) & 0xFF);
  lr->vertices[lr->usedLines * 2 + 1].Colour[2] = (colour & 0xFF);

  lr->hasChanged = true;
  lr->usedLines++;

  return true;
}

bool LineRenderer_AddLine(LineRenderer* lr, const glm::vec3 &p0, const glm::vec3 &p1, const uint32_t &colour)
{
  if (lr->usedLines == lr->maxLines)
    return false;

  lr->vertices[lr->usedLines * 2 + 0].Position[0] = p0.x;
  lr->vertices[lr->usedLines * 2 + 0].Position[1] = p0.y;
  lr->vertices[lr->usedLines * 2 + 0].Position[2] = p0.z;
  lr->vertices[lr->usedLines * 2 + 0].Colour[0] = ((colour >> 16) & 0xFF);
  lr->vertices[lr->usedLines * 2 + 0].Colour[1] = ((colour >> 8) & 0xFF);
  lr->vertices[lr->usedLines * 2 + 0].Colour[2] = (colour & 0xFF);

  lr->vertices[lr->usedLines * 2 + 1].Position[0] = p1.x;
  lr->vertices[lr->usedLines * 2 + 1].Position[1] = p1.y;
  lr->vertices[lr->usedLines * 2 + 1].Position[2] = p1.z;
  lr->vertices[lr->usedLines * 2 + 1].Colour[0] = ((colour >> 16) & 0xFF);
  lr->vertices[lr->usedLines * 2 + 1].Colour[1] = ((colour >> 8) & 0xFF);
  lr->vertices[lr->usedLines * 2 + 1].Colour[2] = (colour & 0xFF);

  lr->hasChanged = true;
  lr->usedLines++;

  return true;
}

void LineRenderer_Render(LineRenderer* lr, const glm::mat4 &wvp)
{
  Camera c = Camera();
  c.Projection = wvp;

  Effect_Apply(lr->lineEffect, &c);

  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);

  glBindVertexArray(lr->vaoID);
  glBindBuffer(GL_ARRAY_BUFFER, lr->bufferID);

  if (!lr->isStatic || lr->hasChanged)
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(LineVertex)* 2 * lr->usedLines, lr->vertices);

  glDrawArrays(GL_LINES, 0, lr->usedLines * 2);

  if (!lr->isStatic)
    lr->usedLines = 0;
}

void LR_CreateBuffers(LineRenderer* lr)
{
  glGenVertexArrays(1, &lr->vaoID);
  glBindVertexArray(lr->vaoID);

  GLenum ErrorCheckValue = glGetError();
  const size_t BufferSize = sizeof(LineVertex)* lr->totalVertices;
  const size_t VertexSize = sizeof(LineVertex);

  glGenBuffers(1, &lr->bufferID);
  glBindBuffer(GL_ARRAY_BUFFER, lr->bufferID);
  glBufferData(GL_ARRAY_BUFFER, BufferSize, lr->vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)(0 * sizeof(float)));
  glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, VertexSize, (GLvoid*)(3 * sizeof(float)));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  ErrorCheckValue = glGetError();
  if (ErrorCheckValue != GL_NO_ERROR)
  {
    fprintf(stderr, "ERROR: Could not create a VBO: %s \n", gluErrorString(ErrorCheckValue));
    exit(-1);
  }
}