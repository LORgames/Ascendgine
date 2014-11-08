#ifndef ENGINECORE_H
#define ENGINECORE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <SDL/SDL.h>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Renderman.h"

const int TICK_INTERVAL = 16;

extern int window_width;
extern int window_height;

//Functions
extern void(*Engine_Resized)(int width, int height);

extern void(*Engine_KeyDown)(int keyCode);
extern void(*Engine_KeyUp)(int keyCode);

extern void(*Engine_MouseDown)(int x, int y, int buttonID);
extern void(*Engine_MouseMove)(int x, int y);
extern void(*Engine_MouseUp)(int x, int y, int buttonID);

extern void(*Engine_TouchDown)(int x, int y, int fingerID, float pressure);
extern void(*Engine_TouchMove)(int x, int y, int fingerID, float pressure);
extern void(*Engine_TouchUp)(int x, int y, int fingerID, float pressure);

//Control things :)
bool Engine_Init(char* windowTitle, int width, int height);
bool Engine_UpdateAndRender(float* dt = nullptr, float* totalTime = nullptr);
bool Engine_Cleanup();
void Engine_EndFrame();

//Window functions
bool Engine_SetFullscreen(bool fullscreen, bool realFullscreen = false);
void Engine_Maximize();
void Engine_Quit(const char *msg, bool forceQuit = false);
void Engine_SetMinimumWindowSize(int width, int height);

//Other controls
Camera* Engine_GetMainCamera();

#endif