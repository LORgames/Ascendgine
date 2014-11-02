#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <GL\glew.h>

#include "Camera.h"
#include "RenderMaterial.h"
#include "Texture.h"

struct Effect
{
  GLuint id;

  GLuint psDiffuse;
  GLuint psNormals;
  GLuint psSpecular;

  GLint vsModelIndex;
  GLint vsViewIndex;
  GLint vsProjectionIndex;
};

void Effect_CreateFromFile(Effect* ef, const char* vertexFilename, const char * fragmentFilename);
void Effect_CreateFromText(Effect* ef, const char* vertexShaderCode, const char* fragmentShaderCode);
void Effect_Destroy(Effect* ef);
void Effect_Apply(Effect* ef, Camera* = nullptr);
void Effect_ApplyModelMatrix(Effect* ef, glm::mat4x4 modelMatrix);

void Effect_BindMaterial(Effect* ef, RenderMaterial* material);
void Effect_BindTexture(Effect* ef, int slotID);
void Effect_BindTextureAdvanced(Effect* ef, int shaderTextureID, int bindedTextureID);

int Effect_GetUniformID(Effect* ef, char* name);