#version 330 core

//Input Format
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormals;
layout(location = 2) in vec2 vertexTextureUVs;

//Ouput Format
out vec3 ex_Normals;
out vec2 ex_TextureUVs;

//Uniforms
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main(){
	vec4 pos = vec4(vertexPosition_modelspace, 1.0);
	gl_Position = Projection * View * Model * pos;
	ex_Normals = vertexNormals;
	ex_TextureUVs = vertexTextureUVs;
}
