#pragma once

#include <GL/glew.h>

static const GLfloat g_vertex_buffer_data[] = {
   -1.0f, -1.0f, 0.0f,
   1.0f, -1.0f, 0.0f,
   0.0f,  1.0f, 0.0f,
};

class Model {
	private:
		GLuint id;
	public:
		Model(void);
		~Model(void);

		void RenderOpaque();
};

