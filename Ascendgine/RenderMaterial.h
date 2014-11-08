#pragma once

#include <string.h>
#include "Texture.h"

struct RenderMaterial
{
	short flags;
	unsigned char red, green, blue, opacity, specularPower, emission;

	GLuint diffuseTexture;
	GLuint normalsTexture;
	GLuint specularTexture;
};

