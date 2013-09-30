#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

#include <GL\glew.h>

class Effect {
	private:
		GLuint id;
	public:
		Effect(const char*, const char *);
		~Effect(void);
		
		void Apply(void);
};

