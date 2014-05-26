#version 330 core

//Input Format
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormals;
layout(location = 2) in vec2 vertexTextureUVs;

//Ouput Format
out vec2 ex_TextureUVs;

//Uniforms
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main(){
	gl_Position = vec4(vertexPosition_modelspace, 1);
	ex_TextureUVs = vertexTextureUVs;
}
