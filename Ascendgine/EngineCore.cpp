#include "EngineCore.h"

#include "QuadRenderer.h"
#include "Font.h"

//Functions
void(*Engine_Resized)(int width, int height) = nullptr;

void(*Engine_KeyDown)(int keyCode) = nullptr;
void(*Engine_KeyUp)(int keyCode) = nullptr;

void(*Engine_MouseDown)(int x, int y, int buttonID) = nullptr;
void(*Engine_MouseMove)(int x, int y) = nullptr;
void(*Engine_MouseUp)(int x, int y, int buttonID) = nullptr;

void(*Engine_TouchDown)(int x, int y, int fingerID, float pressure) = nullptr;
void(*Engine_TouchMove)(int x, int y, int fingerID, float pressure) = nullptr;
void(*Engine_TouchUp)(int x, int y, int fingerID, float pressure) = nullptr;

int window_width;
int window_height;

Uint32 nextTime;
SDL_Window *window;					// Declare a pointer to an SDL_Window
SDL_GLContext mainContext;		// Our OpenGL Handle
SDL_Event event;							// Our event handle
bool isRunning = true;

void CheckSDLError(int line)
{
	#ifndef NDEBUG
  const char *error = SDL_GetError();
  if (*error != '\0')
  {
    printf("SDL Error: %s\n", error);
    if (line != -1)
      printf(" + line: %i\n", line);
    SDL_ClearError();
  }
	#endif
}

void Engine_Quit(const char *msg, bool forceQuit)
{
  printf("%s: %s\n", msg, SDL_GetError());

  if (forceQuit)
  {
    SDL_Quit();
    exit(1);
  }
  else
  {
    isRunning = false;
  }
}

Uint32 TimeLeftThisFrame()
{
  Uint32 now = SDL_GetTicks();

  if(nextTime <= now)
    return 0;
  else
    return nextTime - now;
}

void ProcessMessageQueue(SDL_Event* event)
{
	while (SDL_PollEvent(event))
  {
		if(event->type == SDL_WINDOWEVENT)
    {
			switch (event->window.event)
      {
				case SDL_WINDOWEVENT_RESIZED:
        {
					printf("Window %d resized to %dx%d\n", event->window.windowID, event->window.data1, event->window.data2);
					window_width = event->window.data1;
					window_height = event->window.data2;
          glViewport(0, 0, window_width, window_height);
          Render_FixCamera(window_width, window_height);
          Render_FixGBuffer(window_width, window_height);

          QuadRenderer::Resized(window_width, window_height);

          Font_ResizeAll(window_width, window_height);

          if (Engine_Resized)
            (*Engine_Resized)(window_width, window_height);
				} break;
			}
		}
    else
    {
			switch (event->type)
      {
				case SDL_KEYDOWN:
          if (Engine_KeyDown)
            (*Engine_KeyDown)(event->key.keysym.sym);
          else
            printf_s("Key press detected (No registered handler!): %i\n", event->key.keysym.scancode);
					break;
				case SDL_KEYUP:
          if (Engine_KeyUp)
            (*Engine_KeyUp)(event->key.keysym.sym);
          else
            printf_s("Key release detected (No registered handler!): %i\n", event->key.keysym.scancode);
					break;
        case SDL_MOUSEMOTION:
          if (Engine_MouseMove)
            (*Engine_MouseMove)(event->motion.x, event->motion.y);
          break;
        case SDL_MOUSEBUTTONDOWN:
          if (Engine_MouseDown)
            (*Engine_MouseDown)(event->motion.x, event->motion.y, event->button.button);
          else
            printf_s("Mouse button pressed (x=%d, y=%d, button=%d)\n", event->motion.x, event->motion.y, event->button.button);
          break;
        case SDL_MOUSEBUTTONUP:
          if (Engine_MouseUp)
            (*Engine_MouseUp)(event->motion.x, event->motion.y, event->button.button);
          else
            printf_s("Mouse button released (x=%d, y=%d, button=%d)\n", event->motion.x, event->motion.y, event->button.button);
          break;
        case SDL_FINGERDOWN:
          if (Engine_TouchDown)
            (*Engine_TouchDown)((int)event->tfinger.dx, (int)event->tfinger.dy, (int)event->tfinger.fingerId, event->tfinger.pressure);
          else
            printf_s("Finger (%d) touched: %5.5f, %5.5f, pressure: %5.5f\n", event->tfinger.fingerId, event->tfinger.dx, event->tfinger.dy, event->tfinger.pressure);
          break;
        case SDL_FINGERUP:
          if (Engine_TouchUp)
            (*Engine_TouchUp)((int)event->tfinger.dx, (int)event->tfinger.dy, (int)event->tfinger.fingerId, event->tfinger.pressure);
          else
            printf_s("Finger (%d) removed: %5.5f, %5.5f, pressure: %5.5f\n", event->tfinger.fingerId, event->tfinger.dx, event->tfinger.dy, event->tfinger.pressure);
          break;
        case SDL_FINGERMOTION:
          if (Engine_TouchMove)
            (*Engine_TouchMove)((int)event->tfinger.dx, (int)event->tfinger.dy, (int)event->tfinger.fingerId, event->tfinger.pressure);
          else
            printf_s("Finger (%d) moved: %5.5f, %5.5f, pressure: %5.5f\n", event->tfinger.fingerId, event->tfinger.dx, event->tfinger.dy, event->tfinger.pressure);
          break;
				case SDL_QUIT:
					printf_s("Quit requested, quitting.\n");
          isRunning = false;
					break;
			}
		}
  }
}

bool Engine_SetFullscreen(bool fullscreen, bool realFullscreen /* = false */)
{
  int result;

  if (fullscreen)
  {
    if (realFullscreen)
    {
      result = SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    }
    else
    {
      result = SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
  }
  else
  {
    result = SDL_SetWindowFullscreen(window, 0);
  }
  
  CheckSDLError(__LINE__);
  return result == 0;
}

void Engine_Maximize()
{
  SDL_MaximizeWindow(window);
}

void Engine_SetMinimumWindowSize(int width, int height)
{
  SDL_SetWindowMinimumSize(window, width, height);
}

Camera* Engine_GetMainCamera()
{
  return Render_GetMainCamera();
}

bool Engine_Init(char* windowTitle, int width, int height)
{
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMECONTROLLER) < 0)			// Initialize SDL2
		Engine_Quit("Unable to initialize SDL");		// Or die on error

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// Create an application window with the following settings:
	window = SDL_CreateWindow(
		windowTitle,							//    window title
		SDL_WINDOWPOS_UNDEFINED,	//    initial x position
		SDL_WINDOWPOS_UNDEFINED,	//    initial y position
		width,									  //    width, in pixels
		height,									  //    height, in pixels
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
	);
  
	// Check that the window was successfully made
	if(window == NULL)
		Engine_Quit("Could not create window: %s");

	CheckSDLError(__LINE__);

	mainContext = SDL_GL_CreateContext(window);
  CheckSDLError(__LINE__);
	
	glewExperimental = GL_TRUE;
	glewInit();

  //Debug some things :)
  printf("Vendor: %s\n", glGetString(GL_VENDOR));
  printf("Renderer: %s\n", glGetString(GL_RENDERER));
  printf("Version: %s\n", glGetString(GL_VERSION));
  printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
  printf("Extensions: %s\n", glGetString(GL_EXTENSIONS));

	// This makes our buffer swap syncronized with the monitor's vertical refresh
  SDL_GL_SetSwapInterval(1);

  // Create our renderer
	Render_Init(width, height);
  QuadRenderer::Resized(width, height);

	//Core loop
	nextTime = SDL_GetTicks() + TICK_INTERVAL;

	return true;
}

bool Engine_UpdateAndRender(float* dt, float* totalTime)
{
  ProcessMessageQueue(&event);

	Render_Render(window);

  SDL_Delay(TimeLeftThisFrame());
  nextTime += TICK_INTERVAL;

  if (dt != nullptr)
    *dt = TICK_INTERVAL / 1000.f;

  if (totalTime != nullptr)
    *totalTime = SDL_GetTicks() / 1000.f;

  return isRunning;
}

void Engine_EndFrame()
{
  //Swap buffers
  SDL_GL_SwapWindow(window);
}

bool Engine_Cleanup()
{
  Render_Cleanup();

  /* Delete our opengl context, destroy our window, and shutdown SDL */
  SDL_GL_DeleteContext(mainContext);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return true;
}