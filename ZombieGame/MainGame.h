#pragma once

#include <Tengine/Window.h>
#include <Tengine/GLSLProgram.h>
#include <Tengine/Camera2D.h>
#include <Tengine/InputManager.h>
#include <Tengine/SpriteBatch.h>

#include "Level.h"
#include "Player.h"
#include "Bullet.h"

class Zombie;

enum class GameState {
	PLAY,
	EXIT
};

class MainGame
{
public:
	MainGame();
	~MainGame();

	/// Runs the game
	void run();

private:
	/// Initializes the core systems
	void initSystems();

	/// Initializes the level and sets up everything
	void initLevel();

	/// Initializes the shaders
	void initShaders();

	/// Main game loop for the program
	void gameLoop();

	/// Updates all agents
	void updateAgents();

	/// Update all bullets
	void updateBullets();

	/// Checks the victory condition
	void checkVictory();

	/// Handles input processing
	void processInput();

	/// Renders the game
	void drawGame();

	/// Member Variables
	Tengine::Window _window; ///< The game window

	Tengine::GLSLProgram _textureProgram; ///< The shader program

	Tengine::InputManager _inputManager; ///< Handles input

	Tengine::Camera2D _camera; ///< Main Camera

	Tengine::SpriteBatch _agentSpriteBatch; ///< Draws all agents

	std::vector<Level*> _levels; ///< vector of all levels

	int _screenWidth, _screenHeight;

	GameState _gameState;

	int _currentLevel;

	Player* _player;
	std::vector<Human*> _humans; ///< Vector of all humans
	std::vector<Zombie*> _zombies; ///< Vector of all zombies
	std::vector<Bullet> _bullets; ///< Vector of all bullets

	int _numHumansKilled; ///< Humans killed by the player
	int _numZombiesKilled; ///< Zombies killed by the player

	float _fps;
};


