#ifndef ENGINECORE_H
#define ENGINECORE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <SDL/SDL.h>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Renderman.h"

const int TICK_INTERVAL = 30;

//Functions
extern void(*keyDown)(int keyCode);
extern void(*keyUp)(int keyCode);
extern void(*mouseDown)(int x, int y);

//Control things :)
bool Engine_Init(char* windowTitle, int width, int height);
bool Engine_UpdateAndRender(float* dt = nullptr, float* totalTime = nullptr);
bool Engine_Cleanup();
void Engine_EndFrame();

//Window functions
int Engine_SetFullscreen(bool fullscren);
void Engine_Maximize();
void Engine_Quit(const char *msg);

//Other controls
Camera* Engine_GetMainCamera();

#endif