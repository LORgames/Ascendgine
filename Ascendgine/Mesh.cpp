#include "Mesh.h"

Mesh::Mesh(RenderMaterial* _mat) {
	Material = _mat;
	canRender = false;
}

Mesh::Mesh(RenderMaterial* _mat, Vertex* _verts, int _totalVerts, int* _indices, int _totalIndices) {
	Material = _mat;
	CreateMesh(_verts, _totalVerts, _indices, _totalIndices);
}

void Mesh::CreateMesh(Vertex* _verts, int _totalVerts, int* _indices, int _totalIndices) {
	vertices = _verts;
	totalVertices = _totalVerts;
	indices = _indices;
	totalIndices = _totalIndices;

	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);
    
	GLenum ErrorCheckValue = glGetError();
	const size_t BufferSize = sizeof(Vertex)*totalVertices;
	const size_t VertexSize = sizeof(Vertex);
	
	glGenBuffers(1, &bufferID);
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	glBufferData(GL_ARRAY_BUFFER, BufferSize, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)(0*sizeof(float)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)(3*sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)(6*sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*totalIndices, indices, GL_STATIC_DRAW);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR) {
		fprintf(stderr, "ERROR: Could not create a VBO: %s \n", gluErrorString(ErrorCheckValue));
		exit(-1);
	}

	canRender = true;
}

Mesh::~Mesh(void) {
	GLenum ErrorCheckValue = glGetError();

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &bufferID);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &indexBufferID);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vaoID);
}

void Mesh::RenderOpaque(Effect* fx) {
	if(!canRender) return;

	glBindVertexArray(vaoID);
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

	fx->BindMaterial(Material);
	
	// Draw the triangles!
	glDrawElements(GL_TRIANGLES, totalIndices, GL_UNSIGNED_INT, 0);
}