#pragma once

namespace Tengine {

	class MainGame;

	enum class ScreenState {
		NONE,
		RUNNING,
		EXIT_APPLICATION,
		CHANGE_NEXT,
		CHANGE_PREVIOUS
	};

	class IGameScreen
	{
	public:
		IGameScreen() {
			// Empty
		}
		~IGameScreen() {
			// Empty
		}

		// Returns the index of the next or previou screen when changing screens
		virtual int getNextScreenIndex() const = 0;
		virtual int getPreviousScreenIndex() const = 0;

		// Called at beginnig and end of application
		virtual void build() = 0;
		virtual void destroy() = 0;

		// Called when a screen enters and exits focus
		virtual void onEntry() = 0;
		virtual void onExit() = 0;

		// Called in the main game loop
		virtual void update() = 0;
		virtual void draw() = 0;

		int getScreenIndex() const {
			return m_screenIndex;
		}

	protected:
		ScreenState m_currentState = ScreenState::NONE;
		MainGame* m_game = nullptr;
		int m_screenIndex = -1;
	};

}

