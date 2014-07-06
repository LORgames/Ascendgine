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
		
    void BindTexture(int slotID)
    {
      if (slotID == 0)
        glUniform1i(psDiffuse, slotID);
      else if (slotID == 1)
        glUniform1i(psNormals, slotID);
      else if (slotID == 2)
        glUniform1i(psSpecular, slotID);
    }
};

