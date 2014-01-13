#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

#include <GL\glew.h>

#include "Camera.h"

class Effect {
	private:
		GLuint id;

		GLint vsModelIndex;
		GLint vsViewIndex;
		GLint vsProjectionIndex;
	public:
		Effect(const char*, const char *);
		~Effect(void);
		
		void Apply(Camera*);
};

