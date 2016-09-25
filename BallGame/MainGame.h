#pragma once
#include <vector>
#include <memory>

#include <Tengine/Camera2D.h>
#include <Tengine/SpriteBatch.h>
#include <Tengine/InputManager.h>
#include <Tengine/Window.h>
#include <Tengine/GLSLProgram.h>
#include <Tengine/Timing.h>
#include <Tengine/SpriteFont.h>

#include "Ball.h"
#include "Grid.h"
#include "BallRenderer.h"
#include "BallController.h"

enum class GameState { RUNNING, EXIT };

const int CELL_SIZE = 12;

class MainGame
{
public:
	~MainGame();

	void run();
private:
	void init();
	void initRenderers();
	void initBalls();
	void update(float deltaTime);
	void draw();
	void drawHud();
	void processInput();

	int m_screenWidth = 0;
	int m_screenHeight = 0;

	std::vector<Ball> m_balls; ///< All the balls
	std::unique_ptr<Grid> m_grid; ///< Grid for spatial partitioning for collision

	int m_currentRenderer = 0;
	std::vector<std::unique_ptr<BallRenderer> > m_ballRenderers;

	BallController m_ballController; ///< Controls balls

	Tengine::Window m_window; ///< The main window
	Tengine::SpriteBatch m_spriteBatch; ///< Renders all the balls
	std::unique_ptr<Tengine::SpriteFont> m_spriteFont; ///< For font rendering
	Tengine::Camera2D m_camera; ///< Renders the scene
	Tengine::InputManager m_inputManager; ///< Handles input
	Tengine::GLSLProgram m_textureProgram; ///< Shaders for textures

	Tengine::FpsLimiter m_fpsLimiter; ///< Limits and calculates fps
	float m_fps = 0.0f;

	GameState m_gameState = GameState::RUNNING; ///< The state of the game
};