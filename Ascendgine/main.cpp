#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <SDL/SDL.h>
#include <glm/glm.hpp>

#include "Renderman.h"

#ifdef _WIN32
#undef main
#endif

#define TICK_INTERVAL	30

int width = 1200, height = 900;
static Uint32 next_time;

bool gameRunning = true;

Renderman* Renderer;

void sdldie(const char *msg) {
    printf("%s: %s\n", msg, SDL_GetError());
    SDL_Quit();
    exit(1);
}

void checkSDLError(int line = -1) {
	#ifndef NDEBUG
        const char *error = SDL_GetError();
        if (*error != '\0') {
                printf("SDL Error: %s\n", error);
                if (line != -1)
                        printf(" + line: %i\n", line);
                SDL_ClearError();
        }
	#endif
}

void Update() {

}

Uint32 time_left(void) {
    Uint32 now;

    now = SDL_GetTicks();
    if(next_time <= now)
        return 0;
    else
        return next_time - now;
}

void ProcessMessageQueue(SDL_Event* event) {
	while (SDL_PollEvent(event)) {
		if(event->type == SDL_WINDOWEVENT) {
			switch (event->window.event) {
				case SDL_WINDOWEVENT_RESIZED: {
					printf("Window %d resized to %dx%d\n", event->window.windowID, event->window.data1, event->window.data2);
					width = event->window.data1;
					height = event->window.data2;
					glViewport(0, 0, width, height); 
					Renderer->FixCamera(width, height);
					Renderer->FixGBuffer(width, height);
				} break;
			}
		} else {
			switch (event->type) {
				case SDL_KEYDOWN:
					//printf_s("Key press detected: %i\n", event->key.keysym.scancode);
					break;
				case SDL_KEYUP:
					printf_s("Key release detected: %i\n", event->key.keysym.scancode);
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
					gameRunning = false;
					break;
			}
		}
    }
}

int main(int argc, char* argv[]) {
    SDL_Window *window;							// Declare a pointer to an SDL_Window
	SDL_GLContext mainContext;					// Our OpenGL Handle
	SDL_Event event;							// Our event handle

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO ) < 0)			// Initialize SDL2
		sdldie("Unable to initialize SDL");		// Or die on error

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// Create an application window with the following settings:
	window = SDL_CreateWindow(
		"Ascendgine",							//    window title
		SDL_WINDOWPOS_UNDEFINED,	//    initial x position
		SDL_WINDOWPOS_UNDEFINED,	//    initial y position
		width,									  //    width, in pixels
		height,									  //    height, in pixels
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED
	);
  
	// Check that the window was successfully made
	if(window == NULL){
		sdldie("Could not create window: %s");
	}

	checkSDLError(__LINE__);

	mainContext = SDL_GL_CreateContext(window);
    checkSDLError(__LINE__);
	
	glewExperimental = GL_TRUE;
	glewInit();

  
  //Debug some things :)
  printf("Vendor: %s\n", glGetString(GL_VENDOR));
  printf("Renderer: %s\n", glGetString(GL_RENDERER));
  printf("Version: %s\n", glGetString(GL_VERSION));
  printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
  printf("Extensions: %s\n", glGetString(GL_EXTENSIONS));


	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	/* This makes our buffer swap syncronized with the monitor's vertical refresh */
  SDL_GL_SetSwapInterval(1);

	//Setup device state :)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glEnable(GL_DEPTH_TEST);

	Renderer = new Renderman(width, height);

	/* Game Loop */
	next_time = SDL_GetTicks() + TICK_INTERVAL;
    while (gameRunning) {
		ProcessMessageQueue(&event);

        Update();
		Renderer->Render(window);

        SDL_Delay(time_left());
        next_time += TICK_INTERVAL;
    }

  	delete Renderer;

    /* Delete our opengl context, destroy our window, and shutdown SDL */
    SDL_GL_DeleteContext(mainContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

	return 0;
}