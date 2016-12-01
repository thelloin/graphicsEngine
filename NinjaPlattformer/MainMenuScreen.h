#pragma once

#include <Tengine/IGameScreen.h>

#include "Player.h"
#include <vector>
#include <Tengine/SpriteBatch.h>
#include <Tengine/GLSLProgram.h>
#include <Tengine/Camera2D.h>
#include <Tengine/GLTexture.h>
#include <Tengine/Window.h>
#include <Tengine/DebugRenderer.h>

#include <Tengine/GUI.h>

class MainMenuScreen : public Tengine::IGameScreen
{
public:
	MainMenuScreen(Tengine::Window* window);
	~MainMenuScreen();

	// Returns the index of the next or previou screen when changing screens
	virtual int getNextScreenIndex() const override;
	virtual int getPreviousScreenIndex() const override;

	// Called at beginning and end of application
	virtual void build() override;
	virtual void destroy() override;

	// Called when a screen enters and exits focus
	virtual void onEntry() override;
	virtual void onExit() override;

	// Called in the main game loop
	virtual void update() override;
	virtual void draw() override;
private:
	void initUI();
	void checkInput();

	bool onNewGameClicked(const CEGUI::EventArgs& e);
	bool onExitClicked(const CEGUI::EventArgs& e);

	Tengine::Camera2D m_camera;
	Tengine::Window* m_window;
	Tengine::GUI m_gui;
	
};