#include "Model.h"


Model::Model(void) {
	
}

Model::~Model(void) {
	for(int i = 0; i < TotalMeshes; i++) {
		delete Meshes[i];
	}

	for(int i = 0; i < TotalMaterials; i++) {
		delete Materials[i];
	}

	delete[] Meshes;
	delete[] Materials;
}

void Model::LoadFromFile(char* filename) {
	std::ifstream file (filename, std::ios::in|std::ios::binary|std::ios::ate);

	std::streampos size;
	char * memblock;

	fprintf(stdout, "Loading Mesh: %s\n", filename);

	if(file.is_open()) {
		size = file.tellg();
		memblock = new char[(unsigned int)size];

		file.seekg(0, std::ios::beg);
		file.read(memblock, size);
		file.close();

		BinaryReaderX f = BinaryReaderX(memblock);

		unsigned short version = f.ReadUnsignedShort();
		short embeddedinfo = f.ReadShort();

		int totalVerts = f.ReadInt();
		int totalIndcs = f.ReadInt();

		TotalMaterials = f.ReadUnsignedShort();
		TotalMeshes = f.ReadUnsignedShort();

		fprintf(stdout, "\tLGM Version: v%i\n\tVertices: %i\n\tIndices: %i\n\tMaterials: %i\n\tMeshes: %i\n", version, totalVerts, totalIndcs, TotalMaterials, TotalMeshes);

		Materials = new RenderMaterial*[TotalMaterials];
		for(int i = 0; i < TotalMaterials; i++) {
			Materials[i] = new RenderMaterial();

			Materials[i]->flags = f.ReadShort();

			Materials[i]->red = f.ReadByte();
			Materials[i]->green = f.ReadByte();
			Materials[i]->blue = f.ReadByte();
			Materials[i]->opacity = f.ReadByte();
			Materials[i]->specularPower = f.ReadByte();

			fprintf(stdout, "\tReading Material: %i/%i\n\t\tRGBA: %i, %i, %i, %i\n", (i+1), TotalMaterials, Materials[i]->red, Materials[i]->green, Materials[i]->blue, Materials[i]->opacity);

			if((Materials[i]->flags & (1 << 0)) > 0) {
				Materials[i]->diffuseTextureFilename = f.ReadString();
			}
			
			if((Materials[i]->flags & (1 << 1)) > 0) {
				Materials[i]->normalTextureFilename = f.ReadString();
			}
			
			if((Materials[i]->flags & (1 << 2)) > 0) {
				Materials[i]->specularTextureFilename = f.ReadString();
			}
		}

		Meshes = new Mesh*[TotalMeshes];
		for(int i = 0; i < TotalMeshes; i++) {
			short MatID = f.ReadShort();

			int totalVertices = f.ReadInt();
			int totalIndices = f.ReadInt();

			short flags = f.ReadShort();

			Vertex* _verts = new Vertex[totalVertices];
			int* _indices = new int[totalIndices];

			bool _Normals = ((flags & (1)) > 0);
			bool _Unwrapd = ((flags & (2)) > 0);

			fprintf(stdout, "\tReading Mesh: %i/%i\n\t\tVertices: %i\n\t\tIndices: %i\n", (i+1), TotalMeshes, totalVertices, totalIndices);

			for (int j = 0; j < totalVertices; j++) {
				_verts[j].Position[0] = f.ReadFloat();
				_verts[j].Position[1] = f.ReadFloat();
				_verts[j].Position[2] = f.ReadFloat();
				_verts[j].Normals[0] = f.ReadFloat();
				_verts[j].Normals[1] = f.ReadFloat();
				_verts[j].Normals[2] = f.ReadFloat();
				_verts[j].UVs[0] = f.ReadFloat();
				_verts[j].UVs[1] = f.ReadFloat();
			}

			for(int j = 0; j < totalIndices; j++) {
				_indices[j] = f.ReadInt();
			}

			Meshes[i] = new Mesh(Materials[MatID], _verts, totalVertices, _indices, totalIndices);
		}

		delete[] memblock;

		fprintf(stdout, "\tFlushed memory. %i bytes\n\n", (unsigned int)size);
	} else {
		fprintf(stdout, "\tFAILED! Could not open file!\n\n");
	}
}

void Model::RenderOpaque(void) {
	//for(int i = 0; i < TotalMeshes; i++) {
	//	Meshes[i]->RenderOpaque();
	//}

	Meshes[0]->RenderOpaque();
}