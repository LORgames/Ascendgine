#include "Texture.h"
#include <stb_image.c> //Need this for image loading :)

Texture::Texture(void) {

}


Texture::~Texture(void) {

}

void Texture::LoadTexture(const char* expectedPath, const char* filename)
{
	printf_s("Loading texture '%s' (trying path: %s)... ", filename, expectedPath);

	size_t pathLen = strlen(expectedPath);
	size_t fileLen = strlen(filename);

	char* fullLocation = new char[(pathLen + fileLen + 1)];
	memcpy(fullLocation, expectedPath, pathLen);
	memcpy(fullLocation+pathLen, filename, fileLen);

	fullLocation[pathLen+fileLen] = 0;

  int depth;
	unsigned char* data = stbi_load(fullLocation, &m_width, &m_height, &depth, 0);

	if(data) {
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if(depth == 3)
    {
      m_colorMode = GL_RGB;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			printf_s("Loaded!\n");
    }
    else if (depth == 4)
    {
      m_colorMode = GL_RGBA;
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      printf_s("Loaded!\n");
    }
    else
    {
			printf_s("Failed (depth)!\n");
		}
	} else {
		printf_s("Failed!\n");
	}

	printf_s("\tID:%i, W:%i, H:%i, D:%i\n", textureID, m_width, m_height, depth);

	stbi_image_free(data);
}

void Texture::CreateTexture(int width, int height, GLenum colorMode, char* initialData)
{
	printf_s("Creating empty texture... ");

  m_width = width;
  m_height = height;
  m_colorMode = colorMode;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if(initialData != nullptr)
  {
	  SetData(initialData);
  }
	
	printf_s("\tID:%i, W:%i, H:%i, C:%i\n", textureID, width, height, colorMode);
}

void Texture::SetData(char* data)
{
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, m_colorMode, GL_UNSIGNED_BYTE, data);
}