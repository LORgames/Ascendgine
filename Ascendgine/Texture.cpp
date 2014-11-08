#include "Texture.h"
#include <stb_image.c> //Need this for image loading :)

#include <stdio.h>
#include <fstream>

void Texture_Destroy(GLuint textureID)
{
  glDeleteTextures(1, &textureID);
}

GLuint Texture_Load(const char* filename, int *pWidth, int *pHeight, GLenum *pColourMode)
{
	printf_s("Loading texture '%s'... ", filename);

  int width, height, depth;
  GLuint colourMode;
  GLuint textureID;

  unsigned char* data = stbi_load(filename, &width, &height, &depth, 0);

	if(data)
  {
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

		if(depth == 3)
    {
      colourMode = GL_RGB;
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			printf_s("Loaded!\n");
    }
    else if (depth == 4)
    {
      colourMode = GL_RGBA;
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      printf_s("Loaded!\n");
    }
    else
    {
			printf_s("Failed (depth)!\n");
		}
	}
  else
  {
		printf_s("Failed!\n");
	}

  printf_s("\tID:%i, W:%i, H:%i, D:%i\n", textureID, width, height, depth);
	stbi_image_free(data);

  if (pWidth) *pWidth = width;
  if (pHeight) *pHeight = height;
  if (pColourMode) *pColourMode = colourMode;

  return textureID;
}

GLuint Texture_LoadFromPath(const char* filename, const char* path, int *pWidth, int *pHeight, GLenum *pColourMode)
{
  printf_s("Loading texture '%s' (from path: %s)... ", filename, path);

  size_t pathLen = strlen(path);
  size_t fileLen = strlen(filename);

  char* fullLocation = new char[(pathLen + fileLen + 2)];
  strcpy(fullLocation, path);
  strcat(fullLocation, "\\");
  strcat(fullLocation, filename);

  GLuint textureID = Texture_Load(fullLocation, pWidth, pHeight, pColourMode);
  delete[] fullLocation;

  return textureID;
}

GLuint Texture_CreateTexture(int width, int height, GLenum colorMode, char* initialData)
{
	printf_s("Creating empty texture... ");

  GLuint textureID;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

  if(initialData != nullptr)
	  Texture_SetData(textureID, initialData, width, height, colorMode);
	
	printf_s("\tID:%i, W:%i, H:%i, C:%i\n", textureID, width, height, colorMode);

  return textureID;
}

void Texture_SetData(GLuint textureID, char* data, int m_width, int m_height, GLenum m_colorMode)
{
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, m_colorMode, GL_UNSIGNED_BYTE, data);
}