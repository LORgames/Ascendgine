#include "Model.h"


Model::Model(void) {
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);
    
	GLenum ErrorCheckValue = glGetError();
	const size_t BufferSize = sizeof(Vertices);
	const size_t VertexSize = sizeof(Vertices[0]);
	const size_t RgbOffset = sizeof(Vertices[0].XYZ);
	
	glGenBuffers(1, &bufferID);
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	glBufferData(GL_ARRAY_BUFFER, BufferSize, Vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VertexSize, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	//TODO: Figure out a better error solution for this project
	//ErrorCheckValue = glGetError();
	//if (ErrorCheckValue != GL_NO_ERROR) {
	//	fprintf(stderr, "ERROR: Could not create a VBO: %s \n", gluErrorString(ErrorCheckValue));
	//	exit(-1);
	//}
}

Model::~Model(void) {
	GLenum ErrorCheckValue = glGetError();

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &bufferID);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &indexBufferID);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vaoID);

	//TODO: Better Error Reporting System
	//ErrorCheckValue = glGetError();
	//if (ErrorCheckValue != GL_NO_ERROR) {
	//	fprintf(stderr, "ERROR: Could not destroy the VBO: %s \n", gluErrorString(ErrorCheckValue));
	//	exit(-1);
	//}
}

void Model::RenderOpaque(void) {
	// Draw the triangles!
							  //VVVV 48 = Faces*3 (total indices)
	glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_BYTE, 0);
}
