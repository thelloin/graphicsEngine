#pragma once

#include <SDL/SDL.h>
#include <GL/glew.h>

#include <Tengine/Tengine.h>
#include <Tengine/GLSLProgram.h>
#include <Tengine/GLTexture.h>
#include <Tengine/Sprite.h>
#include <Tengine/Window.h>
#include <Tengine/InputManager.h>
#include <Tengine/Timing.h>

#include <Tengine/Camera2D.h>
#include <Tengine/SpriteBatch.h>

#include "Bullet.h"

#include <vector>

enum class GameState {PLAY, EXIT};

class MainGame
{
public:
	MainGame();
	~MainGame();

	// Runs the game
	void run();

private:
	// Initializes the core systems
	void initSystems();

	// Initializes the shaders
	void initShaders();

	// Main game loop for the program
	void gameLoop();

	// Handles input processing
	void processInput();

	// Renders the game
	void drawGame();

	Tengine::Window _window;
	int _screenWidth;
	int _screenHeight;
	GameState _gameState;

	Tengine::GLSLProgram _colorProgram;
	Tengine::Camera2D _camera;

	Tengine::SpriteBatch _spriteBatch;

	Tengine::InputManager _inputManager;
	Tengine::FpsLimiter _fpsLimiter;

	std::vector<Bullet> _bullets;

	float _maxFPS;
	float _fps;
	float _time;
};

