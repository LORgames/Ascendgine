#include "Renderman.h"

#include "Quad.h"
#include "Sphere.h"

enum BUFFER_TYPE {
  BUFFER_COLOUR,
  BUFFER_NORMAL,
  BUFFER_DEPTH,
  TOTAL_BUFFERS
};

//Rendering to GBuffer
Effect* fxOpaque;
Effect* fxTransparent;
Effect* fxAnimated;

//Rendering to Screen
Effect* fxPostProcessing;

//Rendering lights
Effect* fxLightPoint;
Effect* fxLightDirectional;

//RenderTargets
GLuint fbo;						//FRAME BUFFER OBJECT
GLuint InputRT[TOTAL_BUFFERS];	//GBUFFER TEXTURES

GLuint Lightfbo;    //Lighting Frame Buffer
GLuint LightRT;     //Lighting Texture

//Stored objects
Mesh* screenQuad;
Camera* mainCam;
Mesh* lightingSphere;

std::vector<Model*> Render_Models;

void Render_Init(int width, int height)
{
	fxOpaque = new Effect("../shaders/OpaqueShader.vs", "../shaders/OpaqueShader.ps");
  fxPostProcessing = new Effect("../shaders/QuadRenderer.vs", "../shaders/QuadRenderer.ps");
  fxLightPoint = new Effect("../shaders/PointLight.vs", "../shaders/PointLight.ps");

	mainCam = new Camera();
	mainCam->Projection = glm::mat4();
	mainCam->View = glm::mat4();
	mainCam->Model = glm::mat4();

	Render_FixCamera(width, height);
	Render_FixGBuffer(width, height);
	
	glClearColor(1.0, 1.0, 1.0, 1.0);

  screenQuad = new Quad();
  screenQuad->SetEffect(fxPostProcessing);

  lightingSphere = new Sphere();
}

void Render_Cleanup(void)
{
	delete fxOpaque;
  delete fxPostProcessing;
  delete fxLightPoint;

	delete mainCam;

  delete lightingSphere;

  delete screenQuad;
}

void Render_Render(SDL_Window* window)
{
	//Fix the states for opaque rendering
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glDepthMask(GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  //Apply the camera
	fxOpaque->Apply(mainCam);

	//Render the models
  for (int i = 0; i < (int)Render_Models.size(); i++)
  {
    Render_Models[i]->RenderOpaque(1);
  }
	
	//Fix the states for light rendering
  glBindFramebuffer(GL_FRAMEBUFFER, Lightfbo);
	glEnable(GL_BLEND);
	//glBlendEquation(GL_FUNC_ADD);
	//glBlendFunc(GL_ONE, GL_ONE);
	glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  fxLightPoint->Apply(mainCam);
  fxLightPoint->ApplyModelMatrix(glm::scale(glm::mat4(), glm::vec3(10.f, 10.f, 10.f)));
  lightingSphere->RenderOpaque();

  //And do the blending :)
  fxPostProcessing->Apply();

	//Apply the textures
	for (unsigned int i = 0; i < TOTAL_BUFFERS; i++)
  {
	  glActiveTexture(GL_TEXTURE0 + i);
	  glBindTexture(GL_TEXTURE_2D, InputRT[i]);
    
    fxPostProcessing->BindTexture(i);
	}

  //Upload the lighting map
  glActiveTexture(GL_TEXTURE0 + 4);
  glBindTexture(GL_TEXTURE_2D, LightRT);
  fxPostProcessing->BindTexture(4);

  //Render to screen :)
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  screenQuad->RenderOpaque();
}

void Render_FixCamera(int width, int height)
{
	mainCam->CreatePerspectiveProjection((float)width, (float)height, 30, 0.1f, 2500.0f);
	mainCam->View = glm::lookAt(glm::vec3(50,50,250), glm::vec3(0,0,0), glm::vec3(0,1,0));
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,NULL);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, InputRT[BUFFER_DEPTH], 0);

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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, LightRT, 0);

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
  return fxOpaque;
}

Camera* Render_GetMainCamera()
{
  return mainCam;
}

GLuint Render_GetBuffer(int id)
{
  return InputRT[id];
}

/*void Renderman_DrawPointLight(glm::vec3 lightPosition, int color, float lightRadius, float lightIntensity)
{
  //compute the light world matrix
  //scale according to light radius, and translate it to light position
  glm::mat4x4 sphereWorldMatrix = glm::scale(glm::mat4x4(), glm::vec3(lightRadius, lightRadius, lightRadius)) * glm::translate(glm::mat4x4(), lightPosition);
  fxLightPoint->ApplyModelMatrix(sphereWorldMatrix);

  //light position
  fxLightPoint->GetUniformID("lightPosition");
  glUniform3f(fxLightPoint->GetUniformID("lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);

  //set the color, radius and Intensity
  EffectLightsPoint.Parameters["Color"].SetValue(color.ToVector3());
  glUniform1f(fxLightPoint->GetUniformID("lightRadius"), lightRadius);
  glUniform1f(fxLightPoint->GetUniformID("lightIntensity"), lightIntensity);

  //parameters for specular computations
  EffectLightsPoint.Parameters["cameraPosition"].SetValue(CharacterCamera.Position);
  EffectLightsPoint.Parameters["InvertViewProjection"].SetValue(Matrix.Invert(CharacterCamera.View * CharacterCamera.Projection));

  //calculate the distance between the camera and light center
  float cameraToCenter = Vector3.Distance(CharacterCamera.Position, lightPosition);

  //if we are inside the light volume, draw the sphere's inside face
  if (cameraToCenter < lightRadius)
    Game.device.RasterizerState = RasterizerState.CullClockwise;
  else
    Game.device.RasterizerState = RasterizerState.CullCounterClockwise;

  Game.device.DepthStencilState = DepthStencilState.None;

  EffectLightsPoint.Techniques[0].Passes[0].Apply();

  foreach(ModelMesh mesh in SphereModel.Meshes) {
    foreach(ModelMeshPart meshPart in mesh.MeshParts) {
      Game.device.Indices = meshPart.IndexBuffer;
      Game.device.SetVertexBuffer(meshPart.VertexBuffer);
      Game.device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, meshPart.NumVertices, meshPart.StartIndex, meshPart.PrimitiveCount);
    }
  }

  Game.device.RasterizerState = RasterizerState.CullCounterClockwise;
  Game.device.DepthStencilState = DepthStencilState.Default;
}*/