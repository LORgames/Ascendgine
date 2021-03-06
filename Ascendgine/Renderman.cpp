﻿#include "Renderman.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Effect.h"
#include "Quad.h"
#include "Sphere.h"

enum BUFFER_TYPE {
  BUFFER_COLOUR,
  BUFFER_NORMAL,
  BUFFER_DEPTH,
  TOTAL_BUFFERS
};

//Rendering to GBuffer
Effect fxOpaque;
Effect fxTransparent;
Effect fxAnimated;

//Rendering to Screen
Effect fxPostProcessing;

//Rendering lights
Effect fxLightPoint;
Effect fxLightDirectional;

//RenderTargets
GLuint fbo;						//FRAME BUFFER OBJECT
GLuint InputRT[TOTAL_BUFFERS];	//GBUFFER TEXTURES

GLuint Lightfbo;    //Lighting Frame Buffer
GLuint LightRT;     //Lighting Texture

//Light blending effect
GLuint Light_DepthMap;

//Stored objects
Mesh* screenQuad;
Camera* mainCam;
Mesh* lightingSphere;

int fxpp_lightMapID = 0;

std::vector<Model*> Render_Models;
std::vector<LightInfo> Render_Lights;

//Forward declare this :)
void Renderman_DrawPointLight(glm::vec3 lightPosition, glm::vec3 color, float lightRadius, float lightIntensity);

void Render_Init(int width, int height)
{
	Effect_CreateFromFile(&fxOpaque, "../shaders/OpaqueShader.vs", "../shaders/OpaqueShader.ps");
  Effect_CreateFromFile(&fxPostProcessing, "../shaders/QuadRenderer.vs", "../shaders/QuadRenderer.ps");
  Effect_CreateFromFile(&fxLightPoint, "../shaders/PointLight.vs", "../shaders/PointLight.ps");

  fxpp_lightMapID = Effect_GetUniformID(&fxPostProcessing, "lightQQ");
  Light_DepthMap = Effect_GetUniformID(&fxLightPoint, "depthMap");

	mainCam = new Camera();
	mainCam->Projection = glm::mat4();
	mainCam->View = glm::mat4();
	mainCam->Model = glm::mat4();

	Render_FixCamera(width, height);
	Render_FixGBuffer(width, height);
	
	glClearColor(1.0, 1.0, 1.0, 1.0);

  screenQuad = Create_Quad();
  Mesh_SetEffect(screenQuad, &fxPostProcessing);

  lightingSphere = Create_Sphere();
  glClearColor(0, 0, 0, 0);
}

void Render_Cleanup(void)
{
	Effect_Destroy(&fxOpaque);
  Effect_Destroy(&fxPostProcessing);
  Effect_Destroy(&fxLightPoint);

	delete mainCam;
  delete lightingSphere;
  delete screenQuad;
}

void Render_Render(SDL_Window* window)
{
  if (Render_Models.size() == 0)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return;
  }

	//Fix the states for opaque rendering
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glDepthMask(GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  glDisable(GL_BLEND);

  //Apply the camera
  Effect_Apply(&fxOpaque, mainCam);

	//Render the models
  for (int i = (int)Render_Models.size()-1; i >= 0; i--)
  {
    if (Render_Models[i]->renderFunction != nullptr)
      Render_Models[i]->renderFunction(Render_Models[i], RenderPass_Standard);
  }

	//Fix the states for light rendering
  glBindFramebuffer(GL_FRAMEBUFFER, Lightfbo);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ONE);
	glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Effect_Apply(&fxLightPoint, mainCam);

  //Apply the textures
  for (unsigned int i = 0; i < TOTAL_BUFFERS; i++)
  {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, InputRT[i]);

    if (i != BUFFER_DEPTH)
      Effect_BindTexture(&fxLightPoint, i);
    else
      Effect_BindTextureAdvanced(&fxLightPoint, Light_DepthMap, i);
  }

  for (unsigned int i = 0; i < Render_Lights.size(); i++)
  {
    Renderman_DrawPointLight(Render_Lights[i].position, Render_Lights[i].colour, Render_Lights[i].radius, Render_Lights[i].intensity);
  }

  //And do the blending :)
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_BLEND);

  Effect_Apply(&fxPostProcessing);

  //Upload the lighting map
  glActiveTexture(GL_TEXTURE0 + 2);
  glBindTexture(GL_TEXTURE_2D, LightRT);
  Effect_BindTextureAdvanced(&fxPostProcessing, fxpp_lightMapID, 2);

	//Apply the textures
	for (unsigned int i = 0; i < TOTAL_BUFFERS-1; i++)
  {
	  glActiveTexture(GL_TEXTURE0 + i);
	  glBindTexture(GL_TEXTURE_2D, InputRT[i]);
    
    Effect_BindTexture(&fxPostProcessing, i);
	}

  //Render to screen :)
  Mesh_RenderOpaque(screenQuad);
}

void Render_FixCamera(int width, int height)
{
  Camera_CreatePerspectiveProjection(mainCam, (float)width, (float)height, 30, 100.0f, 2500.0f);
	mainCam->View = glm::lookAt(glm::vec3(50,50,250), glm::vec3(0,100,0), glm::vec3(0,1,0));
	mainCam->Model = glm::mat4();
}

void Render_FixGBuffer(int width, int height)
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	// generate texture object
	glGenTextures(TOTAL_BUFFERS, InputRT);
	for (unsigned int i = 0; i < BUFFER_DEPTH; i++)
  {
    glBindTexture(GL_TEXTURE_2D, InputRT[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, InputRT[i], 0);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }

	// generate depth texture object
	glBindTexture(GL_TEXTURE_2D, InputRT[BUFFER_DEPTH]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, InputRT[BUFFER_DEPTH], 0);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 }; 
  glDrawBuffers(2, DrawBuffers);

	//Check if its ready
	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (Status != GL_FRAMEBUFFER_COMPLETE) { printf_s("MRT-FBO error, status: 0x%x\n", Status); }

  //Setup the lighting buffer
  glGenFramebuffers(1, &Lightfbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Lightfbo);

  glGenTextures(1, &LightRT);
  glBindTexture(GL_TEXTURE_2D, LightRT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, LightRT, 0);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  GLenum DrawBuffers2[] = { GL_COLOR_ATTACHMENT0 };
  glDrawBuffers(1, DrawBuffers2);

  //Check if its ready
  GLenum Status2 = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (Status2 != GL_FRAMEBUFFER_COMPLETE) { printf_s("LightFBO error, status: 0x%x\n", Status2); }

  //Reset to the default buffer
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

Effect* Render_GetSimpleEffect()
{
  return &fxOpaque;
}

Camera* Render_GetMainCamera()
{
  return mainCam;
}

GLuint Render_GetBuffer(int id)
{
  return InputRT[id];
}

void Renderman_DrawPointLight(glm::vec3 lightPosition, glm::vec3 color, float lightRadius, float lightIntensity)
{
  //compute the light world matrix
  //scale according to light radius, and translate it to light position
  Effect_Apply(&fxLightPoint, mainCam);
  glm::mat4x4 sphereWorldMatrix = glm::translate(glm::mat4x4(), lightPosition) * glm::scale(glm::mat4x4(), glm::vec3(lightRadius, lightRadius, lightRadius));
  Effect_ApplyModelMatrix(&fxLightPoint, sphereWorldMatrix);

  //light position
  glUniform3f(Effect_GetUniformID(&fxLightPoint, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);

  //set the color, radius and Intensity
  glUniform3f(Effect_GetUniformID(&fxLightPoint, "Color"), color.x, color.y, color.z);
  glUniform1f(Effect_GetUniformID(&fxLightPoint, "lightRadius"), lightRadius);
  glUniform1f(Effect_GetUniformID(&fxLightPoint, "lightIntensity"), lightIntensity);

  //parameters for specular computations
  glUniform3f(Effect_GetUniformID(&fxLightPoint, "cameraPosition"), mainCam->Position.x, mainCam->Position.y, mainCam->Position.z);
  glUniformMatrix4fv(Effect_GetUniformID(&fxLightPoint, "InvertViewProjection"), 1, GL_FALSE, glm::value_ptr(glm::inverse(mainCam->Projection * mainCam->View)));

  //calculate the distance between the camera and light center
  float cameraToCenter = glm::distance(mainCam->Position, lightPosition);

  //if we are inside the light volume, draw the sphere's inside face
  if (cameraToCenter < lightRadius)
    glCullFace(GL_FRONT);

  Mesh_RenderOpaque(lightingSphere);

  glCullFace(GL_BACK);
}