﻿#include "Renderman.h"

Renderman::Renderman(int width, int height) {
	testModel = new Model();
	testModel->LoadFromFile("../assets/Map.lgm");

	fxOpaque = new Effect("../shaders/OpaqueShader.vs", "../shaders/OpaqueShader.ps");
	mainCam = new Camera();
	mainCam->Projection = glm::mat4();
	mainCam->View = glm::mat4();
	mainCam->Model = glm::mat4();

	FixCamera(width, height);
	FixGBuffer(width, height);
	
	glClearColor(1.0, 1.0, 1.0, 1.0);
}


Renderman::~Renderman(void) {
	delete testModel;
	delete fxOpaque;
	delete mainCam;
}

void Renderman::Render(SDL_Window* window) {
	//Fix the states for opaque rendering
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

	//Apply the camera
	fxOpaque->Apply(mainCam);

	//Render the models
	testModel->RenderOpaque(fxOpaque, 0);
	
	//Fix the states for light rendering
 glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	glDepthMask(GL_FALSE);
 glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Apply the textures
	for (unsigned int i = 0; i < TOTAL_BUFFERS; i++) {
		glActiveTexture(GL_TEXTURE0 + i);		
		glBindTexture(GL_TEXTURE_2D, InputRT[i]);
	}

	//Diffuse
	glReadBuffer(GL_COLOR_ATTACHMENT0 + BUFFER_COLOUR);
	glBlitFramebuffer(0, 0, 1200, 900, 600, 0, 1200, 450, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	//Normal
	glReadBuffer(GL_COLOR_ATTACHMENT0 + BUFFER_NORMAL);
	glBlitFramebuffer(0, 0, 1200, 900, 0, 0, 600, 450, GL_COLOR_BUFFER_BIT, GL_LINEAR);

 SDL_GL_SwapWindow(window);
}

void Renderman::FixCamera(int width, int height) {
	mainCam->CreatePerspectiveProjection((float)width, (float)height, 30, 0.1f, 1000.0f);
	mainCam->View = glm::lookAt(glm::vec3(250,50,250), glm::vec3(0,0,0), glm::vec3(0,1,0));
	mainCam->Model = glm::mat4();
}

void Renderman::FixGBuffer(int width, int height) {
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	// generate texture object
	glGenTextures(TOTAL_BUFFERS, InputRT);
	for (unsigned int i = 0; i < BUFFER_DEPTH; i++) {
	 glBindTexture(GL_TEXTURE_2D, InputRT[i]);
	 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	 glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, InputRT[i], 0);

		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }

	// generate depth texture object
	glBindTexture(GL_TEXTURE_2D, InputRT[BUFFER_DEPTH]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,NULL);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, InputRT[BUFFER_DEPTH], 0);

  GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 }; 
  glDrawBuffers(2, DrawBuffers);

	// generate output texture object
	//glGenTextures(1, &OutputRT);
	//glBindTexture(GL_TEXTURE_2D, OutputRT);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	//glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, OutputRT, 0);

	//Check if its ready
	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (Status != GL_FRAMEBUFFER_COMPLETE) { printf_s("FB error, status: 0x%x\n", Status); }

	//Reset to the default buffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}