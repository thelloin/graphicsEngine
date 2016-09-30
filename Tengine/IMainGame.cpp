#include "IMainGame.h"
#include "Timing.h"

#include "ScreenList.h"
#include "IGameScreen.h"

namespace Tengine
{

	IMainGame::IMainGame()
	{
		// Empty
	}

	IMainGame::~IMainGame()
	{
		// Empty
	}

	void IMainGame::run()
	{
		if (!init()) return;

		FpsLimiter limiter;
		limiter.setMaxFPS(60.0f);

		m_isRunning = true;
		while (m_isRunning)
		{
			limiter.begin();

			update();
			draw();

			m_fps = limiter.end();
		}
	}

	void IMainGame::exitGame()
	{
		m_currentScreen->onExit();
		if (m_screenList)
		{
			m_screenList->destroy();
			m_screenList.reset();
		}
		m_isRunning = false;
	}

	bool IMainGame::init()
	{
		Tengine::init();

		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

		initSystems();

		onInit();
	}

	bool IMainGame::initSystems()
	{
		m_window.create("Default", 800, 600, 0);

	}

}