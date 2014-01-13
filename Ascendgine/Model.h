#pragma once

#include <GL/glew.h>

static const GLfloat g_vertex_buffer_data[] = {
   -10.0f, -10.0f, 0.0f,
    10.0f, -10.0f, 0.0f,
    00.0f,  10.0f, 0.0f,
};

class Model {
	private:
		GLuint id;
	public:
		Model(void);
		~Model(void);

		void RenderOpaque();
};