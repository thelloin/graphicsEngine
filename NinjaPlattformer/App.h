#pragma once

#include <Tengine/IMainGame.h>

#include "GameplayScreen.h"

class App : public Tengine::IMainGame
{
public:
	App();
	~App();

	// Called on initialization
	virtual void onInit() override;
	// Adds all the screens
	virtual void addScreens() override;
	// Called when exiting
	virtual void onExit() override;
private:
	std::unique_ptr<GameplayScreen> m_gameplayScreen = nullptr;
};

