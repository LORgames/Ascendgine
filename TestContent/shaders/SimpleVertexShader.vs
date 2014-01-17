#version 330 core

//Input Format
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColour;

//Ouput Format
out vec3 ex_Colour;

//Uniforms
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main(){
	vec4 pos = vec4(vertexPosition_modelspace, 1.0f);
	gl_Position = Projection * View * Model * pos;
	ex_Colour = vertexColour;
}
