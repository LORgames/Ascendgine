#pragma once

#include <stdio.h>
#include <fstream>

#include <GL/glew.h>

class Texture {
	private:
		int m_width;
		int m_height;

    GLenum m_colorMode;
	public:
		Texture(void);
		~Texture(void);

		void LoadTexture(const char* expectedPath, const char* filename);
    
    void CreateTexture(int width, int height, GLenum colorMode, char* initialData = nullptr);
    void SetData(char* data);

		GLuint textureID;
};

