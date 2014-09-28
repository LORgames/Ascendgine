#include "Effect.h"

Effect::Effect(const char* vertexFile, const char* fragmentFile)
{
	// Create the shaders
  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
 
  // Read the Vertex Shader code from the file
  std::string VertexShaderCode;
  std::ifstream VertexShaderStream(vertexFile, std::ios::in);
  if(VertexShaderStream.is_open())
  {
    std::string Line = "";
    while(getline(VertexShaderStream, Line))
      VertexShaderCode += "\n" + Line;

    VertexShaderStream.close();
  }

  // Read the Fragment Shader code from the file
  std::string FragmentShaderCode;
  std::ifstream FragmentShaderStream(fragmentFile, std::ios::in);
  if(FragmentShaderStream.is_open())
  {
    std::string Line = "";
    while(getline(FragmentShaderStream, Line))
      FragmentShaderCode += "\n" + Line;
    FragmentShaderStream.close();
  }
 
  GLint Result = GL_FALSE;
  int InfoLogLength;
 
  // Compile Vertex Shader
  printf("Compiling shader : %s\n", vertexFile);
  char const * VertexSourcePointer = VertexShaderCode.c_str();
  glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
  glCompileShader(VertexShaderID);
 
  // Check Vertex Shader
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0)
  {
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    if (VertexShaderErrorMessage[0])
      fprintf(stdout, "Message: %s\n", &VertexShaderErrorMessage[0]);
  }
 
  // Compile Fragment Shader
  printf("Compiling shader : %s\n", fragmentFile);
  char const * FragmentSourcePointer = FragmentShaderCode.c_str();
  glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
  glCompileShader(FragmentShaderID);
 
  // Check Fragment Shader
  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0)
  {
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    if (FragmentShaderErrorMessage[0])
      fprintf(stdout, "Message: %s\n", &FragmentShaderErrorMessage[0]);
  }
 
  // Link the program
  fprintf(stdout, "Linking program\n");
  id = glCreateProgram();
  glAttachShader(id, VertexShaderID);
  glAttachShader(id, FragmentShaderID);
  glLinkProgram(id);
 
  // Check the program
  glGetProgramiv(id, GL_LINK_STATUS, &Result);
  glGetProgramiv(id, GL_INFO_LOG_LENGTH, &InfoLogLength);
  std::vector<char> ProgramErrorMessage(std::max(InfoLogLength, int(1)));
  glGetProgramInfoLog(id, InfoLogLength, NULL, &ProgramErrorMessage[0]);
  fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

  //Get the references
  this->vsModelIndex = glGetUniformLocation(id, "World");
  this->vsViewIndex = glGetUniformLocation(id, "View");
  this->vsProjectionIndex = glGetUniformLocation(id, "Projection");

  this->psDiffuse = glGetUniformLocation(id, "diffuse");
  this->psNormals = glGetUniformLocation(id, "normals");
  this->psSpecular = glGetUniformLocation(id, "specular");

  //Clean up
  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);
}

Effect::~Effect(void)
{
	glDeleteProgram(id);
}

void Effect::Apply(Camera* cam)
{
	glUseProgram(id);

  if(cam)
  {
	  glUniformMatrix4fv(vsModelIndex,		  1, GL_FALSE, glm::value_ptr(cam->Model));
	  glUniformMatrix4fv(vsViewIndex,			  1, GL_FALSE, glm::value_ptr(cam->View));
	  glUniformMatrix4fv(vsProjectionIndex,	1, GL_FALSE, glm::value_ptr(cam->Projection));
  }
  else
  {
    glm::mat4 proj = glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);
    glUniformMatrix4fv(vsProjectionIndex,	1, GL_FALSE, glm::value_ptr(proj));
  }
}

void Effect::ApplyModelMatrix(glm::mat4x4 modelMatrix)
{
  glUniformMatrix4fv(vsModelIndex, 1, GL_FALSE, glm::value_ptr(modelMatrix));
}

void Effect::BindMaterial(RenderMaterial* material)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, material->diffuseTexture->textureID);

	glUniform1i(psDiffuse, 0);

  if(material->normalsTexture)
  {
	  glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, material->normalsTexture->textureID);
	  glUniform1i(psNormals, 1);
  }

  if(material->specularTexture)
  {
	  glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, material->normalsTexture->textureID);
  	glUniform1i(psSpecular, 2);
  }
}

void Effect::BindTextureAdvanced(int shaderTextureID, int bindedTextureID)
{
  glUniform1i(shaderTextureID, bindedTextureID);
}

void Effect::BindTexture(int simpleBindID)
{
  if (simpleBindID == 0)
    glUniform1i(psDiffuse, simpleBindID);
  else if (simpleBindID == 1)
    glUniform1i(psNormals, simpleBindID);
  else if (simpleBindID == 2)
    glUniform1i(psSpecular, simpleBindID);
}

int Effect::GetUniformID(char* name)
{
  return glGetUniformLocation(id, name);
}