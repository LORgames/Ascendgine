#pragma once

#include <GL/glew.h>

typedef struct {
	float XYZ[3];
	float RGB[3];
} Vertex;

const Vertex Vertices[] = {
    { { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
    // Top
    { { -0.2f, 0.8f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
    { { 0.2f, 0.8f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
    { { 0.0f, 0.8f, 0.0f }, { 0.0f, 1.0f, 1.0f } },
    { { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
    // Bottom
    { { -0.2f, -0.8f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
    { { 0.2f, -0.8f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
    { { 0.0f, -0.8f, 0.0f }, { 0.0f, 1.0f, 1.0f } },
    { { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
    // Left
    { { -0.8f, -0.2f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
    { { -0.8f, 0.2f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
    { { -0.8f, 0.0f, 0.0f }, { 0.0f, 1.0f, 1.0f } },
    { { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
    // Right
    { { 0.8f, -0.2f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
    { { 0.8f, 0.2f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
    { { 0.8f, 0.0f, 0.0f }, { 0.0f, 1.0f, 1.0f } },
    { { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } }
};

const GLubyte Indices[] = {
    // Top
    0, 1, 3,
    0, 3, 2,
    3, 1, 4,
    3, 4, 2,
    // Bottom
    0, 5, 7,
    0, 7, 6,
    7, 5, 8,
    7, 8, 6,
    // Left
    0, 9, 11,
    0, 11, 10,
    11, 9, 12,
    11, 12, 10,
    // Right
    0, 13, 15,
    0, 15, 14,
    15, 13, 16,
    15, 16, 14
};

class Model {
	private:
		GLuint vaoID;
		GLuint bufferID;
		GLuint indexBufferID;
	public:
		Model(void);
		~Model(void);

		void RenderOpaque();
};