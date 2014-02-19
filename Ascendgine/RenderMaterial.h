#pragma once

#include <string.h>

class RenderMaterial {
public:
	short flags;
	char red, green, blue, opacity, specularPower;

	std::string diffuseTextureFilename;
	std::string normalTextureFilename;
	std::string specularTextureFilename;

	RenderMaterial(void) {

	}

	~RenderMaterial(void) {

	}
};

