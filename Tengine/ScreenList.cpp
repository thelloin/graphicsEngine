#include "ScreenList.h"

#include "IMainGame.h"
#include "IGameScreen.h"

namespace Tengine {

	ScreenList::ScreenList(IMainGame* game) :
		m_game(game)
	{

	}

	IGameScreen* ScreenList::moveNext()
	{
		IGameScreen* currentScreen = getCurrent();
		if (currentScreen->getNextScreenIndex() != SCREEN_INDEX_NO_INDEX)
		{
			m_currentScreenIndex = currentScreen->getNextScreenIndex();
		}
		return getCurrent();
	}

	IGameScreen* ScreenList::movePrevious()
	{
		IGameScreen* currentScreen = getCurrent();
		if (currentScreen->getPreviousScreenIndex() != SCREEN_INDEX_NO_INDEX)
		{
			m_currentScreenIndex = currentScreen->getPreviousScreenIndex();
		}
		return getCurrent();
	}

	void ScreenList::setScreen(int nextScreen)
	{
		m_currentScreenIndex = nextScreen;
	}

	void ScreenList::addScreen(IGameScreen* nextScreen)
	{
		m_screens.push_back(nextScreen);
		nextScreen->build();
		nextScreen->setParentGame(m_game);
	}

	void ScreenList::destroy()
	{
		for (size_t i = 0; i < m_screens.size(); i++)
		{
			m_screens[i]->destroy();
			delete m_screens[i];
		}
		m_screens.resize(0);
		m_currentScreenIndex = SCREEN_INDEX_NO_INDEX;
	}

	ScreenList::~ScreenList()
	{
		destroy();
	}

	IGameScreen* ScreenList::getCurrent()
	{
		if (m_currentScreenIndex == SCREEN_INDEX_NO_INDEX)
			return nullptr;
		return m_screens[m_currentScreenIndex];
	}

}