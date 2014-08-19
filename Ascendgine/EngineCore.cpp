#include "EngineCore.h"

#include "QuadRenderer.h"

//Functions
void (*keyDown)(int keyCode) = nullptr;
void (*keyUp)(int keyCode) = nullptr;
void (*mouseDown)(int x, int y) = nullptr;

int width;
int height;

Uint32 nextTime;
Renderman* renderer;
SDL_Window *window;					// Declare a pointer to an SDL_Window
SDL_GLContext mainContext;		// Our OpenGL Handle
SDL_Event event;							// Our event handle
bool isRunning = true;

void EngineCore::CheckSDLError(int line)
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

void EngineCore::Quit(const char *msg)
{
  printf("%s: %s\n", msg, SDL_GetError());
  SDL_Quit();
  exit(1);
}

Uint32 EngineCore::TimeLeftThisFrame()
{
  Uint32 now = SDL_GetTicks();

  if(nextTime <= now)
    return 0;
  else
    return nextTime - now;
}

void EngineCore::ProcessMessageQueue(SDL_Event* event)
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
					width = event->window.data1;
					height = event->window.data2;
					glViewport(0, 0, width, height); 
					renderer->FixCamera(width, height);
					renderer->FixGBuffer(width, height);

          QuadRenderer::Resized(width, height);
				} break;
			}
		}
    else
    {
			switch (event->type)
      {
				case SDL_KEYDOWN:
          if (keyDown)
            (*keyDown)(event->key.keysym.scancode);
          else
            printf_s("Key press detected (No registered handler!): %i\n", event->key.keysym.scancode);
					break;
				case SDL_KEYUP:
          if (keyUp)
            (*keyUp)(event->key.keysym.scancode);
          else
            printf_s("Key release detected (No registered handler!): %i\n", event->key.keysym.scancode);
					break;
				case SDL_MOUSEBUTTONDOWN:
					printf_s("Mouse button pressed\n");
					break;
        case SDL_FINGERDOWN:
          printf_s("Finger (%d) touched: %5.5f, %5.5f, pressure: %5.5f\n", event->tfinger.fingerId, event->tfinger.dx, event->tfinger.dy, event->tfinger.pressure);
          break;
        case SDL_FINGERUP:
          printf_s("Finger (%d) removed: %5.5f, %5.5f, pressure: %5.5f\n", event->tfinger.fingerId, event->tfinger.dx, event->tfinger.dy, event->tfinger.pressure);
          break;
        case SDL_FINGERMOTION:
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

void EngineCore::Maximize()
{
  SDL_MaximizeWindow(window);
}

Renderman* EngineCore::GetRenderer()
{
  return renderer;
}

Camera* EngineCore::GetMainCamera()
{
  return renderer->GetMainCamera();
}

bool EngineCore::Init(char* windowTitle, int width, int height)
{
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO ) < 0)			// Initialize SDL2
		Quit("Unable to initialize SDL");		// Or die on error

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
		Quit("Could not create window: %s");

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
	renderer = new Renderman(width, height);
  QuadRenderer::Resized(width, height);

	//Core loop
	nextTime = SDL_GetTicks() + TICK_INTERVAL;

	return true;
}

bool EngineCore::UpdateAndRender(float* dt, float* totalTime)
{
  ProcessMessageQueue(&event);

	renderer->Render(window);

  SDL_Delay(TimeLeftThisFrame());
  nextTime += TICK_INTERVAL;

  if (dt != nullptr)
    *dt = TICK_INTERVAL / 1000.f;

  if (totalTime != nullptr)
    *totalTime = SDL_GetTicks() / 1000.f;

  return isRunning;
}

void EngineCore::EndFrame()
{
  //Swap buffers
  SDL_GL_SwapWindow(window);
}

bool EngineCore::Cleanup()
{
  delete renderer;

  /* Delete our opengl context, destroy our window, and shutdown SDL */
  SDL_GL_DeleteContext(mainContext);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return true;
}