#include "Texture.h"
#include <stb_image.c> //Need this for image loading :)

Texture::Texture(void) {

}


Texture::~Texture(void) {

}

void Texture::LoadTexture(const char* expectedPath, const char* filename) {
	printf_s("Loading texture '%s' (trying path: %s)... ", filename, expectedPath);

	size_t pathLen = strlen(expectedPath);
	size_t fileLen = strlen(filename);

	char* fullLocation = new char[(pathLen + fileLen + 1)];
	memcpy(fullLocation, expectedPath, pathLen);
	memcpy(fullLocation+pathLen, filename, fileLen);

	fullLocation[pathLen+fileLen] = 0;

	unsigned char* data = stbi_load(fullLocation, &width, &height, &depth, 0);

	if(data) {
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if(depth == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			printf_s("Loaded!\n");
		} else if(depth == 4) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			printf_s("Loaded!\n");
		} else {
			printf_s("Failed (depth)!\n");
		}
	} else {
		printf_s("Failed!\n");
	}

	printf_s("\tID:%i, W:%i, H:%i, D:%i\n", textureID, width, height, depth);

	stbi_image_free(data);
}