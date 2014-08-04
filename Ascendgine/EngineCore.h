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

class EngineCore
{
private:
  //Functions
  static void ProcessMessageQueue(SDL_Event* event);
  static void CheckSDLError(int line = -1);
  static Uint32 TimeLeftThisFrame();
public:
  //Control things :)
  static bool Init(char* windowTitle, int width, int height);
  static bool UpdateAndRender();
  static bool Cleanup();
  static void EndFrame();

  //Window functions
  static int SetFullscreen(bool fullscren);
  static void Maximize();
  static void Quit(const char *msg);

  //Other controls
  static Renderman* GetRenderer();
  static Camera* GetMainCamera();
};

#endif