#pragma once

#include <Tengine/IGameScreen.h>

#include <Box2D/Box2D.h>
#include "Box.h"
#include "Player.h"
#include <vector>
#include <Tengine/SpriteBatch.h>
#include <Tengine/GLSLProgram.h>
#include <Tengine/Camera2D.h>
#include <Tengine/GLTexture.h>
#include <Tengine/Window.h>
#include <Tengine/DebugRenderer.h>

#include <Tengine/GUI.h>

class GameplayScreen : public Tengine::IGameScreen
{
public:
	GameplayScreen(Tengine::Window* window);
	~GameplayScreen();

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
	void checkInput();

	Tengine::SpriteBatch m_spriteBatch;
	Tengine::GLSLProgram m_textureProgram;
	Tengine::GLSLProgram m_lightProgram;
	Tengine::Camera2D m_camera;
	Tengine::GLTexture m_texture;

	Tengine::Window* m_window;
	Tengine::GUI m_gui;

	Tengine::DebugRenderer m_debugRenderer;
	bool m_renderDebug = false;
	bool m_lightsOn = false;

	Player m_player;
	std::vector<Box> m_boxes;
	std::unique_ptr<b2World> m_world;
};

