#include "Model.h"

#include "EngineCore.h"

Model::Model(void) {
  TotalMeshes = 0;
  TotalMaterials = 0;
}

Model::~Model(void) {
	for(int i = 0; i < TotalMeshes; i++) {
		delete Meshes[i];
	}

	for(int i = 0; i < TotalMaterials; i++) {
		delete Materials[i];
	}

  if (TotalMeshes > 0)
    delete[] Meshes;

  if (TotalMaterials > 0)
    delete[] Materials;
}

void Model::LoadFromFile(char* filename)
{
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

    TotalMaterials = BRX_ReadUnsignedShort(&f);
    TotalMeshes = BRX_ReadUnsignedShort(&f);

		fprintf(stdout, "\tLGM Version: v%i\n\tVertices: %i\n\tIndices: %i\n\tMaterials: %i\n\tMeshes: %i\n", version, totalVerts, totalIndcs, TotalMaterials, TotalMeshes);

		Materials = new RenderMaterial*[TotalMaterials];
		for(int i = 0; i < TotalMaterials; i++) {
			Materials[i] = new RenderMaterial();

      Materials[i]->flags = BRX_ReadShort(&f);

      Materials[i]->red = BRX_ReadByte(&f);
      Materials[i]->green = BRX_ReadByte(&f);
      Materials[i]->blue = BRX_ReadByte(&f);
      Materials[i]->opacity = BRX_ReadByte(&f);
      Materials[i]->specularPower = BRX_ReadByte(&f);

			//printf_s("\tReading Material: %i/%i\n\t\tRGBA: %i, %i, %i, %i\n", (i+1), TotalMaterials, Materials[i]->red, Materials[i]->green, Materials[i]->blue, Materials[i]->opacity);

			if((Materials[i]->flags & (1 << 0)) > 0)
      {
        char* textureName = BRX_ReadCharString(&f);
				Materials[i]->diffuseTexture = new Texture();
        Materials[i]->diffuseTexture->LoadTextureFromPath(textureName, expectedPath);
				delete[] textureName;
			}
			
			if((Materials[i]->flags & (1 << 1)) > 0)
      {
        char* textureName = BRX_ReadCharString(&f);
				Materials[i]->normalsTexture = new Texture();
        Materials[i]->normalsTexture->LoadTextureFromPath(textureName, expectedPath);
				delete[] textureName;
			}
			
			if((Materials[i]->flags & (1 << 2)) > 0)
      {
        char* textureName = BRX_ReadCharString(&f);
				Materials[i]->specularTexture = new Texture();
        Materials[i]->specularTexture->LoadTextureFromPath(textureName, expectedPath);
				delete[] textureName;
			}
		}

		Meshes = new Mesh*[TotalMeshes];
		for(int i = 0; i < TotalMeshes; i++)
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

			Meshes[i] = new Mesh(Materials[MatID], _verts, totalVertices, _indices, totalIndices);
      Meshes[i]->SetEffect(Render_GetSimpleEffect());
		}

		delete[] memblock;
    BRX_Destroy(&f);

		fprintf(stdout, "\tFlushed memory. %i bytes\n\n", (unsigned int)size);
	}
  else
  {
		fprintf(stdout, "\tFAILED! Could not open file!\n\n");
	}

	delete[] expectedPath;
}

void Model::RenderOpaque(int passID)
{
	if(passID == 1) //Opaque
  {
		for(int i = 0; i < TotalMeshes; i++)
    {
			Meshes[i]->RenderOpaque();
		}
	}
  else
  {
		
	}
}