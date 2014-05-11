#version 330 core

//Input Format
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormals;
layout(location = 2) in vec2 vertexTextureUVs;

//Ouput Format
out vec2 ex_TextureUVs;
out vec3 ex_Normals;
out vec4 ex_WorldPosition;

//Uniforms
uniform mat4 World;
uniform mat4 View;
uniform mat4 Projection;

void main(){
	//Just copy UVs
	ex_TextureUVs = vertexTextureUVs;

	//Calculate the matrix thing
	mat4 matWVP = Projection * View * World;
	gl_Position = matWVP * vec4(vertexPosition_modelspace, 1.0f);
	ex_WorldPosition = World * vec4(vertexPosition_modelspace, 1.0f);
	
	ex_Normals = normalize((World * vec4(vertexNormals, 0.0f)).xyz);
}
