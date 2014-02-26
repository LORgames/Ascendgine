#pragma once

#include <string.h>
#include "Texture.h"

class RenderMaterial {
public:
	short flags;
	unsigned char red, green, blue, opacity, specularPower, emission;

	Texture* diffuseTexture;
	Texture* normalsTexture;
	Texture* specularTexture;

	RenderMaterial(void) {

	}

	~RenderMaterial(void) {

	}
};

