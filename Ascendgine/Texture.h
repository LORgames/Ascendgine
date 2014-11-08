#pragma once

#include <GL/glew.h>

void Texture_Destroy(GLuint textureID);

GLuint Texture_Load(const char* filename, int *width = nullptr, int *height = nullptr, GLenum *colourMode = nullptr);
GLuint Texture_LoadFromPath(const char* filename, const char* path, int *pWidth = nullptr, int *pHeight = nullptr, GLenum *pColourMode = nullptr);
    
GLuint Texture_CreateTexture(int width, int height, GLenum colorMode, char* initialData = nullptr);
void Texture_SetData(GLuint textureID, char* data, int m_width, int m_height, GLenum m_colorMode);