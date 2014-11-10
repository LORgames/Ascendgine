#include "Mesh.h"

#include "EngineCore.h"

void Mesh_Create(Mesh** ppMesh, RenderMaterial* _mat, Vertex* _verts, int _totalVerts, int* _indices, int _totalIndices)
{
  *ppMesh = new Mesh();
  Mesh* mesh = *ppMesh;

  mesh->Material = _mat;
  mesh->fx = { 0 };

  mesh->vertices = _verts;
  mesh->totalVertices = _totalVerts;
  mesh->indices = _indices;
  mesh->totalIndices = _totalIndices;

  glGenVertexArrays(1, &mesh->vaoID);
  glBindVertexArray(mesh->vaoID);
    
	GLenum ErrorCheckValue = glGetError();
  const size_t BufferSize = sizeof(Vertex)*mesh->totalVertices;
	const size_t VertexSize = sizeof(Vertex);
	
  glGenBuffers(1, &mesh->bufferID);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->bufferID);
  glBufferData(GL_ARRAY_BUFFER, BufferSize, mesh->vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)(0*sizeof(float)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)(3*sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)(6*sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

  glGenBuffers(1, &mesh->indexBufferID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBufferID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*mesh->totalIndices, mesh->indices, GL_STATIC_DRAW);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
  {
		fprintf(stderr, "ERROR: Could not create a VBO: %s \n", gluErrorString(ErrorCheckValue));
		exit(-1);
	}
}

void Mesh_Destroy(Mesh* mesh)
{
	GLenum ErrorCheckValue = glGetError();

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &mesh->bufferID);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &mesh->indexBufferID);

	glBindVertexArray(0);
  glDeleteVertexArrays(1, &mesh->vaoID);

  delete mesh;
}

void Mesh_RenderOpaque(Mesh* mesh)
{
  glBindVertexArray(mesh->vaoID);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->bufferID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBufferID);

  if (mesh->Material)
    Effect_BindMaterial(&mesh->fx, mesh->Material);
	
	// Draw the triangles!
  glDrawElements(GL_TRIANGLES, mesh->totalIndices, GL_UNSIGNED_INT, 0);
}