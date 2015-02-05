#include "Model.h"

#include "EngineCore.h"

void Model_Create(Model **ppModel)
{
  *ppModel = new Model();
  (*ppModel)->TotalMeshes = 0;
  (*ppModel)->TotalMaterials = 0;
}

void Model_Destroy(Model* pModel)
{
  for (int i = 0; i < pModel->TotalMeshes; i++)
  {
    delete pModel->Meshes[i];
	}

  for (int i = 0; i < pModel->TotalMaterials; i++)
  {
    delete pModel->Materials[i];
	}

  if (pModel->TotalMeshes > 0)
    delete[] pModel->Meshes;

  if (pModel->TotalMaterials > 0)
    delete[] pModel->Materials;

  delete pModel;
}

void Model_LoadFromFile(Model** ppModel, char* filename)
{
  *ppModel = new Model();
  Model* model = *ppModel;

	std::ifstream file (filename, std::ios::in|std::ios::binary|std::ios::ate);

	std::streampos size;
	char * memblock;

	fprintf(stdout, "Loading Mesh: %s\n", filename);
	
	//Setup the strings we'll need
	char* filewithext = strrchr(filename, '/')+sizeof(char);
	char* expectedPath = new char[filewithext-filename+1];
	memcpy(expectedPath, filename, sizeof(char)*(filewithext-filename));
	expectedPath[filewithext-filename] = 0;

	//Open file and read data into memory
	if(file.is_open()) {
		size = file.tellg();
		memblock = new char[(unsigned int)size];

		file.seekg(0, std::ios::beg);
		file.read(memblock, size);
		file.close();

    BinaryReaderX f;
    BRX_Create(&f, memblock);

		unsigned short version = BRX_ReadUnsignedShort(&f);
    short embeddedinfo = BRX_ReadShort(&f);

    int totalVerts = BRX_ReadInt(&f);
    int totalIndcs = BRX_ReadInt(&f);

    model->TotalMaterials = BRX_ReadUnsignedShort(&f);
    model->TotalMeshes = BRX_ReadUnsignedShort(&f);

    fprintf(stdout, "\tLGM Version: v%i\n\tVertices: %i\n\tIndices: %i\n\tMaterials: %i\n\tMeshes: %i\n", version, totalVerts, totalIndcs, model->TotalMaterials, model->TotalMeshes);

    model->Materials = new RenderMaterial*[model->TotalMaterials];
    for (int i = 0; i < model->TotalMaterials; i++) {
      model->Materials[i] = new RenderMaterial();

      model->Materials[i]->flags = BRX_ReadShort(&f);

      model->Materials[i]->red = BRX_ReadByte(&f);
      model->Materials[i]->green = BRX_ReadByte(&f);
      model->Materials[i]->blue = BRX_ReadByte(&f);
      model->Materials[i]->opacity = BRX_ReadByte(&f);
      model->Materials[i]->specularPower = BRX_ReadByte(&f);

			//printf_s("\tReading Material: %i/%i\n\t\tRGBA: %i, %i, %i, %i\n", (i+1), TotalMaterials, Materials[i]->red, Materials[i]->green, Materials[i]->blue, Materials[i]->opacity);

      if ((model->Materials[i]->flags & (1 << 0)) > 0)
      {
        char* textureName = BRX_ReadCharString(&f);
        model->Materials[i]->diffuseTexture = Texture_LoadFromPath(textureName, expectedPath);
				delete[] textureName;
			}
			
      if ((model->Materials[i]->flags & (1 << 1)) > 0)
      {
        char* textureName = BRX_ReadCharString(&f);
        model->Materials[i]->normalsTexture = Texture_LoadFromPath(textureName, expectedPath);
				delete[] textureName;
			}
			
      if ((model->Materials[i]->flags & (1 << 2)) > 0)
      {
        char* textureName = BRX_ReadCharString(&f);
        model->Materials[i]->specularTexture = Texture_LoadFromPath(textureName, expectedPath);
				delete[] textureName;
			}
		}

    model->Meshes = new Mesh*[model->TotalMeshes];
    for (int i = 0; i < model->TotalMeshes; i++)
    {
      short MatID = BRX_ReadShort(&f);

      int totalVertices = BRX_ReadInt(&f);
      int totalIndices = BRX_ReadInt(&f);

      short flags = BRX_ReadShort(&f);

			Vertex* _verts = new Vertex[totalVertices];
			int* _indices = new int[totalIndices];

			bool _Normals = ((flags & (1)) > 0);
			bool _Unwrapd = ((flags & (2)) > 0);

			//printf_s("\tReading Mesh: %i/%i\n\t\tVertices: %i\n\t\tIndices: %i\n", (i+1), TotalMeshes, totalVertices, totalIndices);

			for (int j = 0; j < totalVertices; j++) {
        _verts[j].Position[0] = BRX_ReadFloat(&f);
        _verts[j].Position[1] = BRX_ReadFloat(&f);
        _verts[j].Position[2] = BRX_ReadFloat(&f);

        if (_Normals)
        {
          _verts[j].Normals[0] = BRX_ReadFloat(&f);
          _verts[j].Normals[1] = BRX_ReadFloat(&f);
          _verts[j].Normals[2] = BRX_ReadFloat(&f);
        }

        if (_Unwrapd)
        {
          _verts[j].UVs[0] = BRX_ReadFloat(&f);
          _verts[j].UVs[1] = BRX_ReadFloat(&f);
        }
				//printf_s("Vertex %i: %f, %f, %f, %f, %f, %f, %f, %f\n", j, _verts[j].Position[0], _verts[j].Position[1], _verts[j].Position[2], _verts[j].Normals[0], _verts[j].Normals[1], _verts[j].Normals[2], _verts[j].UVs[0], _verts[j].UVs[1]);
			}

			for(int j = 0; j < totalIndices; j++)
      {
        _indices[j] = BRX_ReadInt(&f);
				//printf_s("Index %i: %i\n", j, _indices[j]);
			}

      Mesh_Create(&model->Meshes[i], model->Materials[MatID], _verts, totalVertices, _indices, totalIndices);
      Mesh_SetEffect(model->Meshes[i], Render_GetSimpleEffect());
		}

		delete[] memblock;
    BRX_Destroy(&f);

		fprintf(stdout, "\tFlushed memory. %i bytes\n\n", (unsigned int)size);

    (*ppModel)->renderFunction = Model_RenderOpaque;
	}
  else
  {
		fprintf(stdout, "\tFAILED! Could not open file!\n\n");
	}

	delete[] expectedPath;
}

void Model_RenderOpaque(Model* pModel, int passID)
{
	if(passID == RenderPass_Standard)
  {
		for(int i = 0; i < pModel->TotalMeshes; i++)
    {
      Mesh_RenderOpaque(pModel->Meshes[i]);
		}
	}
  else
  {
		
	}
}