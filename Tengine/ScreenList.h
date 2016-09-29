#pragma once

#include <vector>

namespace Tengine {

	class MainGame;
	class IGameScreen;

	class ScreenList
	{
	public:
		ScreenList(MainGame* game);

		IGameScreen* moveNext();
		IGameScreen* movePrevious();

		void setScreen(int nextScreen);
		void addScreen(IGameScreen* nextScreen);

		void destroy();

	protected:
		MainGame* m_game = nullptr;
		std::vector<IGameScreen*> m_screen;
		int m_currentScreenIndex = -1;
	};

}