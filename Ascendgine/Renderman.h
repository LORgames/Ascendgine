#pragma once

#include <GL/glew.h>
#include <SDL/SDL.h>
#include <glm/glm.hpp>

#include "Model.h"
#include "Camera.h"
#include "Effect.h"

#include <vector>
extern std::vector<Model*> Render_Models;

void Render_Init(int width, int height);
void Render_Cleanup(void);

void Render_Render(SDL_Window* window);

void Render_FixCamera(int width, int height);
void Render_FixGBuffer(int width, int height);

Effect* Render_GetSimpleEffect();
Camera* Render_GetMainCamera();

GLuint Render_GetBuffer(int id);

void Render_DrawPointLight(glm::vec3 lightPosition, int color, float lightRadius, float lightIntensity);

