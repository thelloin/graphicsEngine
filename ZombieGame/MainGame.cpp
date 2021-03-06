#include "MainGame.h"

#include <Tengine/Tengine.h>
#include <Tengine/Timing.h>
#include <Tengine/TengineErrors.h>
#include <Tengine/ResourceManager.h>
#include <random>
#include <ctime>
#include <algorithm>

#include <SDL/SDL.h>
#include <iostream>
#include <glm/gtx/rotate_vector.hpp>

#include "Gun.h"
#include "Zombie.h"

const float HUMAN_SPEED = 1.0f;
const float ZOMBIE_SPEED = 1.3f;
const float PLAYER_SPEED = 5.0f;

MainGame::MainGame() :
	_screenWidth(1024),
	_screenHeight(768),
	_gameState(GameState::PLAY),
	_fps(0),
	_player(nullptr),
	_numHumansKilled(0),
	_numZombiesKilled(0)
{
	// Empty
}

MainGame::~MainGame() {
	// Delete the levels
	for (int i = 0; i < _levels.size(); i++) {
		delete _levels[i];
	}
	// Delete the humans
	for (int i = 0; i < _humans.size(); i++) {
		delete _humans[i];
	}
	// Delete the zombies
	for (int i = 0; i < _zombies.size(); i++) {
		delete _zombies[i];
	}
}

void MainGame::run() {
	initSystems();

	initLevel();

	// Play music, this code should be moved to a better place..
	Tengine::Music music = m_audioEngine.loadMusic("Sound/XYZ.ogg");
	music.play(-1);

	gameLoop();
}

// Custom update function for blood particles
void updateBloodParticle(Tengine::Particle2D& particle, float deltaTime)
{
	particle.position += particle.velocity * deltaTime;
	particle.color.a = (GLubyte)(particle.life * 255.0f);
}

void MainGame::initSystems() {
	// Initialize the game engine
	Tengine::init();

	// Initialize sound, must happen after Tengine::init
	m_audioEngine.init();

	// Create the game window
	_window.create("ZombieGame", _screenWidth, _screenHeight, 0);
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f); /// Grey background color

	// Set up the shaders
	initShaders();

	// Initialize our spritebatch
	_agentSpriteBatch.init();
	_hudSpriteBatch.init();

	// Initialize sprite font
	_spriteFont = new Tengine::SpriteFont("Fonts/chintzy.ttf", 64);

	// Set up the camera
	_camera.init(_screenWidth, _screenHeight);
	_hudCamera.init(_screenWidth, _screenHeight);
	_hudCamera.setPosition(glm::vec2(_screenWidth / 2, _screenHeight / 2));

	// Initialize particles
	m_bloodParticleBatch = new Tengine::ParticleBatch2D();
	m_bloodParticleBatch->init(1000,
							   0.07f,
							   Tengine::ResourceManager::getTexture("Textures/particle.png"),
							   [](Tengine::Particle2D& particle, float deltaTime) {
		particle.position += particle.velocity * deltaTime;
		particle.color.a = (GLubyte)(particle.life * 255.0f);
	});
	m_particleEngine.addParticleBatch(m_bloodParticleBatch);
}

void MainGame::initLevel() {
	// Level 1
	_levels.push_back(new Level("Levels/level1.txt"));
	_currentLevel = 0;

	_player = new Player();
	_player->init(PLAYER_SPEED,
				  _levels[_currentLevel]->getStartPlayerPos(),
				  &_inputManager,
				  &_camera,
				  &_bullets);

	_humans.push_back(_player);

	std::mt19937 randomEngine;
	randomEngine.seed(time(nullptr));
	std::uniform_int_distribution<int> randX(2, _levels[_currentLevel]->getWidth() - 2);
	std::uniform_int_distribution<int> randY(2, _levels[_currentLevel]->getHeight() - 2);

	// Add all the random humans
	for (int i = 0; i < _levels[_currentLevel]->getNumHumans(); i++) {
		_humans.push_back(new Human);
		glm::vec2 pos(randX(randomEngine) * TILE_WIDTH, randY(randomEngine) * TILE_WIDTH);
		_humans.back()->init(HUMAN_SPEED, pos);
	}

	// Add the zombies
	const std::vector<glm::ivec2>& zombiePositions = _levels[_currentLevel]->getZombiesStartPos();
	for (int i = 0; i < zombiePositions.size(); i++) {
		_zombies.push_back(new Zombie);
		_zombies.back()->init(ZOMBIE_SPEED, zombiePositions[i]);
	}

	// Set up the players guns
	const float BULLET_SPEED = 20.0f;
	_player->addGun(new Gun("Magnum", 10, 1, 5.0f, 30, BULLET_SPEED, m_audioEngine.loadSoundEffect("Sound/shots/pistol.wav")));
	_player->addGun(new Gun("Shotgun", 30, 12, 20.0f, 4, BULLET_SPEED, m_audioEngine.loadSoundEffect("Sound/shots/shotgun.wav")));
	_player->addGun(new Gun("MP5", 2, 1, 10.0f, 20, BULLET_SPEED, m_audioEngine.loadSoundEffect("Sound/shots/cg1.wav")));
}

void MainGame::initShaders() {
	// Compile our color shader
	_textureProgram.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
	_textureProgram.addAttribute("vertexPosition");
	_textureProgram.addAttribute("vertexColor");
	_textureProgram.addAttribute("vertexUV");
	_textureProgram.linkShaders();
}

void MainGame::gameLoop() {

	// Some helpful constants
	const float DESIRED_FPS = 60.0F; // FPS the game is designed to run at
	const int MAX_PHYSICS_STEPS = 6; // Max number of physics steps per frame
	const float MS_PER_SECOND = 1000; // Number of milliseconds in one second
	const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS; // The desired frame time per frame
	const float MAX_DELTA_TIME = 1.0f; // Maximum size of deltaTime

	// Used to cap the FPS
	Tengine::FpsLimiter fpsLimiter;
	fpsLimiter.setMaxFPS(600.0f);

	// Zoom out the camera by 3x
	const float CAMERA_SCALE = 1.0f / 3.0f;
	_camera.setScale(CAMERA_SCALE);

	// Start our previousTicks variable
	float previousTicks = SDL_GetTicks();
	
	// Main loop
	while (_gameState == GameState::PLAY) {
		fpsLimiter.begin();

		// Calculate the frameTime in milliseconds
		float newTicks = SDL_GetTicks();
		float frameTime = newTicks - previousTicks;
		previousTicks = newTicks; // Store newTicks in previousTicks so we can use it next frame
		// Get the total delta time
		float totalDeltaTime = frameTime / DESIRED_FRAMETIME;

		checkVictory();

		_inputManager.update();

		processInput();

		int i = 0; // This counter makes sure we don't spiral to death
		// Loop while we still have steps to process
		while (totalDeltaTime > 0.0f && i < MAX_PHYSICS_STEPS) {
			// The deltaTime should be the smallest of totalDeltaTime and MAX_DELTA_TIME
			float deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);
			// Update all physics here and pass in deltaTime
			updateAgents(deltaTime);
			updateBullets(deltaTime);
			m_particleEngine.update(deltaTime);

			totalDeltaTime -= deltaTime;
			i++;
		}

		

		// Make sure the camera is bound to the player position
		_camera.setPosition(_player->getPosition());
		_camera.update();

		_hudCamera.update();

		drawGame();

		// End the frame, limit the FPS, and get the current FPS
		_fps = fpsLimiter.end();
		std::cout << _fps << std::endl; // Print FPS for debugging
	}
}

void MainGame::updateAgents(float deltaTime) {
	// Update all humans
	for (int i = 0; i < _humans.size(); i++) {
		_humans[i]->update(_levels[_currentLevel]->getLevelData(),
						   _humans,
						   _zombies,
						   deltaTime);
	}

	// Update all zombies
	for (int i = 0; i < _zombies.size(); i++) {
		_zombies[i]->update(_levels[_currentLevel]->getLevelData(),
							_humans,
							_zombies,
							deltaTime);
	}

	// Update zombie collision
	for (int i = 0; i < _zombies.size(); i++) {
		// Collide with others zombies
		for (int j = i + 1; j < _zombies.size(); j++) {
			_zombies[i]->collideWithAgent(_zombies[j]);
		}
		// Collide with humans
		for (int j = 1; j < _humans.size(); j++) {
			if (_zombies[i]->collideWithAgent(_humans[j])) {
				// Add the new zombie
				_zombies.push_back(new Zombie);
				_zombies.back()->init(ZOMBIE_SPEED, _humans[j]->getPosition());
				// Delete the human
				delete _humans[j];
				_humans[j] = _humans.back();
				_humans.pop_back();
			}
		}

		// Player and zombie collision
		if (_zombies[i]->collideWithAgent(_player)) {
			Tengine::fatalError("YOU LOOSE!");
		}
	}

	// Update human collision
	for (int i = 0; i < _humans.size(); i++) {
		// Collide with other humans
		for (int j = i + 1; j < _humans.size(); j++) {
			_humans[i]->collideWithAgent(_humans[j]);
		}
	}
}

void MainGame::updateBullets(float deltaTime)
{
	// Update and collide with the world
	for (int i = 0; i < _bullets.size();) {
		// If the update returns true, the bullet collided with a wall
		if (_bullets[i].update(_levels[_currentLevel]->getLevelData(), deltaTime)) {
			_bullets[i] = _bullets.back();
			_bullets.pop_back();
		}
		else {
			i++;
		}
	}

	bool wasBulletRemoved;

	// Collide with humans and zombies
	for (int i = 0; i < _bullets.size(); i++) {
		wasBulletRemoved = false;
		// Loop through zombies
		for (int j = 0; j < _zombies.size();) {
			// Check collision
			if (_bullets[i].collideWithAgent(_zombies[j])) {
				// Add blood
				addBlood(_bullets[i].getPosition(), 5);

				// Damage zombie, and kill it if its out of health
				if (_zombies[j]->applyDamage(_bullets[i].getDamage())) {
					delete _zombies[j];
					_zombies[j] = _zombies.back();
					_zombies.pop_back();
					_numZombiesKilled++;
				}
				else {
					j++;
				}

				// Delete the bullet
				_bullets[i] = _bullets.back();
				_bullets.pop_back();
				wasBulletRemoved = true;
				i--; // Make sure we dont skip a bullet
				// Since the bullet is removed, no need to loop through any more zombies
				break;
			}
			else {
				j++;
			}
		}
		// Loop through humans
		if (wasBulletRemoved == false) {
			for (int j = 1; j < _humans.size();) {
				// Check collision
				if (_bullets[i].collideWithAgent(_humans[j])) {
					// Add blood
					addBlood(_bullets[i].getPosition(), 5);

					// Damage human, and kill it if its out of health
					if (_humans[j]->applyDamage(_bullets[i].getDamage())) {
						delete _humans[j];
						_humans[j] = _humans.back();
						_humans.pop_back();
						_numHumansKilled++;
					}
					else {
						j++;
					}

					// Delete the bullet
					_bullets[i] = _bullets.back();
					_bullets.pop_back();
					i--; // Make sure we dont skip a bullet
						 // Since the bullet is removed, no need to loop through any more zombies
					break;
				}
				else {
					j++;
				}
			}
		}
	}
}

void MainGame::checkVictory() {
	// TODO: Support for multiple levels

	// If all zombies are dead we win
	if (_zombies.empty()) {

		std::printf("*** You win!\n You killed %d humans and %d zombies. There are %d/%d civilians remaining.",
					_numHumansKilled, _numZombiesKilled, _humans.size() - 1, _levels[_currentLevel]->getNumHumans());

		Tengine::fatalError("");
	}
}

void MainGame::processInput() {
	SDL_Event evnt;
	//Will keep looping until there are no more events to process
	while (SDL_PollEvent(&evnt)) {
		switch (evnt.type) {
		case SDL_QUIT:
			_gameState = GameState::EXIT;
			break;
		case SDL_MOUSEMOTION:
			_inputManager.setMouseCoords(evnt.motion.x, evnt.motion.y);
			break;
		case SDL_KEYDOWN:
			_inputManager.pressKey(evnt.key.keysym.sym);
			break;
		case SDL_KEYUP:
			_inputManager.releaseKey(evnt.key.keysym.sym);
			break;
		case SDL_MOUSEBUTTONDOWN:
			_inputManager.pressKey(evnt.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			_inputManager.releaseKey(evnt.button.button);
			break;
		}
	}
}

void MainGame::drawGame() {
	// Set the base depth to 1.0
	glClearDepth(1.0);
	// Clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_textureProgram.use();

	// Draw code
	glActiveTexture(GL_TEXTURE0);

	// Make sure the shader uses texture 0
	GLint textureUniform = _textureProgram.getUniformLocation("mySampler");
	glUniform1i(textureUniform, 0);

	// Grab the camera matrix
	glm::mat4 projectionMatrix = _camera.getCameraMatrix();
	GLint pUniform = _textureProgram.getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	// Draw the level
	_levels[_currentLevel]->draw();

	// Begin drawing agents
	_agentSpriteBatch.begin();

	const glm::vec2 AGENT_DIMS(AGENT_RADIUS * 2);

	// Draw the humans
	for (int i = 0; i < _humans.size(); i++) {
		if (_camera.isBoxInView(_humans[i]->getPosition(), AGENT_DIMS))
		{
			_humans[i]->draw(_agentSpriteBatch);
		}
	}

	// Draw the zombies
	for (int i = 0; i < _zombies.size(); i++) {
		if (_camera.isBoxInView(_zombies[i]->getPosition(), AGENT_DIMS))
		{
			_zombies[i]->draw(_agentSpriteBatch);
		}
	}

	// Draw the bullets
	for (int i = 0; i < _bullets.size(); i++) {
		_bullets[i].draw(_agentSpriteBatch);
	}

	// End spritebatch creation
	_agentSpriteBatch.end();
	// Render to the screen
	_agentSpriteBatch.renderBatch();

	// Render the particles
	m_particleEngine.draw(&_agentSpriteBatch);

	// Render the heads up display
	drawHud();

	// Unbind the program
	_textureProgram.unuse();

	// Swap our buffer and draw everything to the screen!
	_window.swapBuffer();
}

void MainGame::drawHud() 
{
	char buffer[256];

	// Grab the hud camera matrix
	glm::mat4 projectionMatrix = _hudCamera.getCameraMatrix();
	GLint pUniform = _textureProgram.getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	_hudSpriteBatch.begin();

	sprintf_s(buffer, "Num Humans: %d", _humans.size());

	_spriteFont->draw(_hudSpriteBatch,
					  buffer, glm::vec2(0, 0),
					  glm::vec2(0.5f),
					  0.0f, 
					  Tengine::ColorRGBA8(255, 255, 255, 255),
					  Tengine::Justification::LEFT);

	sprintf_s(buffer, "Num Zombies: %d", _zombies.size());

	_spriteFont->draw(_hudSpriteBatch,
					  buffer, glm::vec2(0, 36),
					  glm::vec2(0.5f),
					  0.0f,
					  Tengine::ColorRGBA8(255, 255, 255, 255),
					  Tengine::Justification::LEFT);

	_hudSpriteBatch.end();

	_hudSpriteBatch.renderBatch();
}

void MainGame::addBlood(const glm::vec2& position, int numParticles)
{
	static std::mt19937 randEngine(time(nullptr));
	static std::uniform_real_distribution<float> randAngle(0.0f, 2.0f * M_PI);

	glm::vec2 vel(2.0f, 0.0f);
	Tengine::ColorRGBA8 col(255, 0, 0, 255);

	for (int i = 0; i < numParticles; ++i) {
		m_bloodParticleBatch->AddParticle(position, glm::rotate(vel, randAngle(randEngine)), col, 20.0);
	}
}