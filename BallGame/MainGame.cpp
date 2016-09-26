#define _CRT_SECURE_NO_WARNINGS
#include "MainGame.h"

#include <Tengine/Tengine.h>
#include <Tengine/ResourceManager.h>
#include <SDL/SDL.h>
#include <random>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <iostream>

const float DESIRED_FPS = 60.0f; // FPS the game is designed to run at
const int MAX_PHYSICS_STEPS = 6; // max number of physics steps per frame
const float MS_PER_SECOND = 1000.0f; // Number of milliseconds in a second
const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS; // The desired frame time per frame
const float MAX_DELTA_TIME = 1.0f; // Maximum size of deltaTime

MainGame::~MainGame()
{
	// Empty
}

void MainGame::run()
{
	init();
	initBalls();
	// Start the previousTicks variable
	Uint32 previousTicks = SDL_GetTicks();

	// Game loop
	while (m_gameState == GameState::RUNNING)
	{
		m_fpsLimiter.begin();
		processInput();
		// Calculate the frameTime in milliseconds
		Uint32 newTicks = SDL_GetTicks();
		Uint32 frameTime = newTicks - previousTicks;
		previousTicks = newTicks;
		// Get the total delta time
		float totalDeltaTime = (float)frameTime / DESIRED_FRAMETIME;

		int i = 0; // Makes sure no spiral of death occurs
		// Loop while there is still steps to process
		while (totalDeltaTime > 0.0f && i < MAX_PHYSICS_STEPS)
		{
			// The deltaTime should be the smaller of the totalDeltaTime and MAX_DELTA_TIME
			float deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);

			// Update all physics
			update(deltaTime);
			totalDeltaTime -= deltaTime;
			++i;
		}

		m_camera.update();
		draw();
		m_fps = m_fpsLimiter.end();
	}
}

void MainGame::init()
{
	Tengine::init();
	
	//m_screenWidth = 1890;
	//m_screenHeight = 1010;
	m_screenWidth = 800;
	m_screenHeight = 600;

	m_window.create("Ball Game", m_screenWidth, m_screenHeight, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	m_camera.init(m_screenWidth, m_screenHeight);
	// Point the camera to the center of the screen
	m_camera.setPosition(glm::vec2(m_screenWidth / 2.0f, m_screenHeight / 2.0f));

	m_spriteBatch.init();
	// Initialize sprite font
	m_spriteFont = std::make_unique<Tengine::SpriteFont>("Fonts/chintzy.ttf", 40);

	// Compile the shaders
	m_textureProgram.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
	m_textureProgram.addAttribute("vertexPosition");
	m_textureProgram.addAttribute("vertexColor");
	m_textureProgram.addAttribute("vertexUV");
	m_textureProgram.linkShaders();

	m_fpsLimiter.setMaxFPS(DESIRED_FPS);

	initRenderers();
}

void MainGame::initRenderers()
{
	m_ballRenderers.push_back(std::make_unique<BallRenderer>());
	m_ballRenderers.push_back(std::make_unique<MomentumBallRenderer>());
}

struct BallSpawn
{
	BallSpawn(const Tengine::ColorRGBA8 colr,
			  float rad, float m, float minSpeed,
			  float maxSpeed, float prob) :
		color(colr),
		radius(rad),
		mass(m),
		randSpeed(minSpeed, maxSpeed),
		probability(prob)
	{

	}
	Tengine::ColorRGBA8 color;
	float radius;
	float mass;
	float probability;
	std::uniform_real_distribution<float> randSpeed;
};

void MainGame::initBalls()
{
	// Initialize the grid
	m_grid = std::make_unique<Grid>(m_screenWidth, m_screenHeight, CELL_SIZE);

#define ADD_BALL(p, ...) \
	totalProbability += p; \
	possibleBalls.emplace_back(__VA_ARGS__);

	// Numbers of balls to spawn
	const int NUM_BALLS = 500;

	// Random engine
	std::mt19937 randomEngine((unsigned int)time(nullptr));
	std::uniform_real_distribution<float> randX(0.0f, (float)m_screenWidth);
	std::uniform_real_distribution<float> randY(0.0f, (float)m_screenHeight);
	std::uniform_real_distribution<float> randDir(-1.0f, 1.0f);

	// Add all the balls
	std::vector<BallSpawn> possibleBalls;
	float totalProbability = 0.0f;

	// Random values for ball types
	std::uniform_real_distribution<float> r1(2.0f, 6.0f);
	std::uniform_int_distribution<int> r2(0, 255);

	// Adds the balls using a macro
	ADD_BALL(1.0f, Tengine::ColorRGBA8(255, 255, 255, 255),
			 5.0f, 1.0f, 0.5f, 7.0f, totalProbability);
	ADD_BALL(1.0f, Tengine::ColorRGBA8(0, 0, 255, 255),
			 5.0f, 1.0f, 0.5f, 3.0f, totalProbability);
	ADD_BALL(1.0f, Tengine::ColorRGBA8(0, 255, 0, 255),
			 5.0f, 1.0f, 0.5f, 3.0f, totalProbability);
	//ADD_BALL(1.0f, Tengine::ColorRGBA8(254, 0, 0, 255),
	//		 3.0f, 4.0f, 0.0f, 0.0f, totalProbability);

	// Random probability for ball spawn
	std::uniform_real_distribution<float> spawn(0.0f, totalProbability);

	// Small optimization that sets the size of the interal array to prevent
	// extra allocation, this MUST be here because Grid holds pointers
	// to all the balls, cant resize m_balls then..
	m_balls.reserve(NUM_BALLS);

	// Set up ball to spawn with default value
	BallSpawn* ballToSpawn = &possibleBalls[0];
	for (int i = 0; i < NUM_BALLS; ++i)
	{
		// Get the ball spawn roll
		float spawnVal = spawn(randomEngine);
		// Figure out which ball to pick
		for (size_t j = 0; j < possibleBalls.size(); ++j)
		{
			if (spawnVal <= possibleBalls[j].probability)
			{
				ballToSpawn = &possibleBalls[j];
				break;
			}
		}

		// Get random starting position
		glm::vec2 pos(randX(randomEngine), randY(randomEngine));

		// Get ranodm direction
		glm::vec2 direction(randDir(randomEngine), randDir(randomEngine));
		if (direction.x != 0.0f || direction.y != 0.0f)
		{
			direction = glm::normalize(direction);
		}
		else
		{
			direction = glm::vec2(0.0f);
		}

		// Add the ball
		m_balls.emplace_back(ballToSpawn->radius, ballToSpawn->mass, pos, direction * ballToSpawn->randSpeed(randomEngine),
							 Tengine::ResourceManager::getTexture("Textures/circle.png").id,
							 ballToSpawn->color);
		// Add the ball to the grid.
		m_grid->addBall(&m_balls.back());
	}
}

void MainGame::update(float deltaTime)
{
	m_ballController.updateBalls(m_balls, m_grid.get(), deltaTime, m_screenWidth, m_screenHeight);
}

void MainGame::draw()
{
	// Set the base depth to 1.0;
	glClearDepth(1.0);
	// Clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);

	// Grab the camera matrix
	glm::mat4 projectionMatrix = m_camera.getCameraMatrix();

	m_ballRenderers[m_currentRenderer]->renderBalls(m_spriteBatch, m_balls, projectionMatrix);

	m_textureProgram.use();

	// Make sure the shader uses texture 0
	GLint textureUniform = m_textureProgram.getUniformLocation("mySampler");
	glUniform1i(textureUniform, 0);

	GLint pUniform = m_textureProgram.getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	drawHud();

	m_textureProgram.unuse();

	m_window.swapBuffer();
}

void MainGame::drawHud()
{
	const Tengine::ColorRGBA8 fontColor(255, 0, 0, 255);
	// Convert float to char *
	char buffer[64];
	sprintf(buffer, "%.1f", m_fps);

	m_spriteBatch.begin();
	m_spriteFont->draw(m_spriteBatch, buffer, glm::vec2(0.0f, m_screenHeight - 32.0f),
					   glm::vec2(1.0f), 0.0f, fontColor);
	m_spriteBatch.end();
	m_spriteBatch.renderBatch();
}

void MainGame::processInput()
{
	// Update input manager
	m_inputManager.update();

	SDL_Event evnt;
	// Will loop until there is no more events to process
	while (SDL_PollEvent(&evnt))
	{
		switch (evnt.type)
		{
		case SDLK_ESCAPE:
			m_gameState = GameState::EXIT;
			break;
		case SDL_QUIT:
			m_gameState = GameState::EXIT;
			break;
		case SDL_MOUSEMOTION:
			break;
		case SDL_MOUSEBUTTONDOWN:
			
			break;
		}
	}
	
	if (m_inputManager.isKeyDown(SDLK_ESCAPE))
	{
		m_gameState = GameState::EXIT;
	}
}
