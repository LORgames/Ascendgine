#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

#include <GL\glew.h>

#include "Camera.h"
#include "RenderMaterial.h"
#include "Texture.h"

class Effect {
	private:
		GLuint id;

		GLuint psDiffuse;
		GLuint psNormals;
		GLuint psSpecular;

		GLint vsModelIndex;
		GLint vsViewIndex;
		GLint vsProjectionIndex;
	public:
		Effect(const char*, const char *);
		~Effect(void);
		
		void Apply(Camera* = nullptr);

		void BindMaterial(RenderMaterial* material);
    void BindTexture(int slotID);
    void BindTextureAdvanced(int shaderTextureID, int bindedTextureID);

    int GetUniformID(char* name);
};

