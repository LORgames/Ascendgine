#pragma once

#include <stdio.h>
#include <fstream>

#include <GL/glew.h>

class Texture {
	private:
		int width;
		int height;
		int depth;
	public:
		Texture(void);
		~Texture(void);
		void LoadTexture(const char* expectedPath, const char* filename);

		GLuint textureID;
};

