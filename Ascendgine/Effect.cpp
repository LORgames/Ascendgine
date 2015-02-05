#include "Effect.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Effect_CreateFromFile(Effect* ef, const char* vertexFile, const char* fragmentFile)
{
  printf("Compiling shader files:\n\t%s\n\t%s\n", vertexFile, fragmentFile);
  
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
 
  Effect_CreateFromText(ef, VertexShaderCode.c_str(), FragmentShaderCode.c_str());
}

void Effect_CreateFromText(Effect* ef, const char* VertexSourcePointer, const char* FragmentSourcePointer)
{
  // Create the shaders
  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

  GLint Result = GL_FALSE;
  int InfoLogLength;

  // Compile Vertex Shader
  printf("Compiling vertex shader (from text)\n");
  glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
  glCompileShader(VertexShaderID);

  // Check Vertex Shader
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0)
  {
    char* VertexShaderErrorMessage = new char[InfoLogLength];
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    if (VertexShaderErrorMessage[0])
      fprintf(stdout, "Message: %s\n", &VertexShaderErrorMessage[0]);
    delete[] VertexShaderErrorMessage;
  }

  // Compile Fragment Shader
  printf("Compiling fragment shader (from text)\n");
  glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
  glCompileShader(FragmentShaderID);

  // Check Fragment Shader
  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0)
  {
    char* FragmentShaderErrorMessage = new char[InfoLogLength];
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    if (FragmentShaderErrorMessage[0])
      fprintf(stdout, "Message: %s\n", &FragmentShaderErrorMessage[0]);
    delete FragmentShaderErrorMessage;
  }

  // Link the program
  fprintf(stdout, "Linking program\n");
  ef->id = glCreateProgram();
  glAttachShader(ef->id, VertexShaderID);
  glAttachShader(ef->id, FragmentShaderID);
  glLinkProgram(ef->id);

  // Check the program
  glGetProgramiv(ef->id, GL_LINK_STATUS, &Result);
  glGetProgramiv(ef->id, GL_INFO_LOG_LENGTH, &InfoLogLength);
  char* ProgramErrorMessage = new char[InfoLogLength>1 ? InfoLogLength : 1];
  glGetProgramInfoLog(ef->id, InfoLogLength, NULL, &ProgramErrorMessage[0]);
  fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
  delete[] ProgramErrorMessage;

  //Get the references
  ef->vsModelIndex = glGetUniformLocation(ef->id, "World");
  ef->vsViewIndex = glGetUniformLocation(ef->id, "View");
  ef->vsProjectionIndex = glGetUniformLocation(ef->id, "Projection");

  ef->psDiffuse = glGetUniformLocation(ef->id, "diffuse");
  ef->psNormals = glGetUniformLocation(ef->id, "normals");
  ef->psSpecular = glGetUniformLocation(ef->id, "specular");

  //Clean up
  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);
}

void Effect_Destroy(Effect* ef)
{
  glDeleteProgram(ef->id);
}

void Effect_Apply(Effect* ef, Camera* cam)
{
  glUseProgram(ef->id);

  if(cam)
  {
    glUniformMatrix4fv(ef->vsModelIndex, 1, GL_FALSE, glm::value_ptr(cam->Model));
    glUniformMatrix4fv(ef->vsViewIndex, 1, GL_FALSE, glm::value_ptr(cam->View));
    glUniformMatrix4fv(ef->vsProjectionIndex, 1, GL_FALSE, glm::value_ptr(cam->Projection));
  }
  else
  {
    glm::mat4 proj = glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);
    glUniformMatrix4fv(ef->vsProjectionIndex, 1, GL_FALSE, glm::value_ptr(proj));
  }
}

void Effect_ApplyModelMatrix(Effect* ef, glm::mat4x4 modelMatrix)
{
  glUniformMatrix4fv(ef->vsModelIndex, 1, GL_FALSE, glm::value_ptr(modelMatrix));
}

void Effect_BindMaterial(Effect* ef, RenderMaterial* material)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, material->diffuseTexture);

  glUniform1i(ef->psDiffuse, 0);

  if(material->normalsTexture)
  {
	  glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, material->normalsTexture);
    glUniform1i(ef->psNormals, 1);
  }

  if(material->specularTexture)
  {
	  glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, material->specularTexture);
    glUniform1i(ef->psSpecular, 2);
  }
}

void Effect_BindTextureAdvanced(Effect* ef, int shaderTextureID, int bindedTextureID)
{
  glUniform1i(shaderTextureID, bindedTextureID);
}

void Effect_BindTexture(Effect* ef, int simpleBindID)
{
  if (simpleBindID == 0)
    glUniform1i(ef->psDiffuse, simpleBindID);
  else if (simpleBindID == 1)
    glUniform1i(ef->psNormals, simpleBindID);
  else if (simpleBindID == 2)
    glUniform1i(ef->psSpecular, simpleBindID);
}

int Effect_GetUniformID(Effect* ef, char* name)
{
  return glGetUniformLocation(ef->id, name);
}