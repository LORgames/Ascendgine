#pragma once

#include <GL/glew.h>
#include <SDL/SDL.h>
#include <glm/glm.hpp>

#include "Model.h"
#include "Camera.h"
#include "Effect.h"

#include <vector>

class Renderman {
private:
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
	GLuint OutputRT;

	//Stored objects
  Mesh* screenQuad;
	Camera* mainCam;
public:
	Renderman(int width, int height);
	~Renderman(void);

	void Render(SDL_Window* window);

	void FixCamera(int width, int height);
	void FixGBuffer(int width, int height);

  Effect* GetSimpleEffect() { return fxOpaque; }
  Camera* GetMainCamera() { return mainCam; }

  std::vector<Model*> models;
};

