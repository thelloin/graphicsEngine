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
	m_gameplayScreen = std::make_unique<GameplayScreen>(&m_window);

	m_screenList->addScreen(m_gameplayScreen.get());
	m_screenList->setScreen(m_gameplayScreen->getScreenIndex());
}

void App::onExit()
{

}
