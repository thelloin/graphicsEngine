#include "App.h"
#include <Tengine/ScreenList.h>



App::App()
{
}


App::~App()
{
}

// Called on initialization
void App::onInit()
{

}

void App::addScreens()
{
	m_mainMenuScreen = std::make_unique<MainMenuScreen>(&m_window);
	m_gameplayScreen = std::make_unique<GameplayScreen>(&m_window);

	m_screenList->addScreen(m_mainMenuScreen.get());
	m_screenList->addScreen(m_gameplayScreen.get());
	m_screenList->setScreen(m_mainMenuScreen->getScreenIndex());
}

void App::onExit()
{

}
