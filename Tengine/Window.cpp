#include "Window.h"
#include "TengineErrors.h"

namespace Tengine {

	Window::Window()
	{
	}


	Window::~Window()
	{
	}

	int Window::create(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlags)
	{
		Uint32 flags = SDL_WINDOW_OPENGL;
		_screenWidth = screenWidth;
		_screenHeight = screenHeight;

		if (currentFlags & INVISIBLE) {
			flags |= SDL_WINDOW_HIDDEN;
		}
		if (currentFlags & FULLSCREEN) {
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}
		if (currentFlags & BORDERLESS) {
			flags |= SDL_WINDOW_BORDERLESS;
		}

		// Open an SDL window
		_sdlWindow = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, flags);
		if (_sdlWindow == nullptr) {
			fatalError("SDL window could not be created!");
		}

		// Set up our OpenGL context
		SDL_GLContext glContext = SDL_GL_CreateContext(_sdlWindow);
		if (glContext == nullptr) {
			fatalError("SDL_GL context could not be created!");
		}

		// Set up glew
		GLenum error = glewInit();
		if (error != GLEW_OK) {
			fatalError("Failed to initialize glew!");
		}

		// check the OpenGL Version
		std::printf("*** OpenGL Version: %s ***\n", glGetString(GL_VERSION));

		// Set the background color to blue
		glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

		// Set VSYNC, 1 = on, 0 = off
		SDL_GL_SetSwapInterval(0);

		// Enable alpha blend
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		return 0;
	}

	void Window::swapBuffer()
	{
		// Swap our buffer and draw everything to the screen
		SDL_GL_SwapWindow(_sdlWindow);
	}

}