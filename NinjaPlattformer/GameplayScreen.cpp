#include "GameplayScreen.h"

#include <Tengine/IMainGame.h>
#include <Tengine/ResourceManager.h>
#include <SDL/SDL.h>
#include <random>
#include <iostream>

#include "Light.h"
#include "ScreenIndices.h"

#define DEBUG_RENDER


GameplayScreen::GameplayScreen(Tengine::Window* window) : m_window(window)
{
	m_screenIndex = SCREEN_INDEX_GAMEPLAY;
	m_renderDebug = false;
	m_lightsOn = true;
}


GameplayScreen::~GameplayScreen()
{
}

int GameplayScreen::getNextScreenIndex() const
{
	return SCREEN_INDEX_NO_SCREEN;
}

int GameplayScreen::getPreviousScreenIndex() const
{
	return SCREEN_INDEX_MAINMENU;
}

void GameplayScreen::build()
{

}

void GameplayScreen::destroy()
{

}

void GameplayScreen::onEntry()
{
	
	b2Vec2 gravity(0.0f, -25.0);
	m_world = std::make_unique<b2World>(gravity);

	// Make the ground
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -23.0f);
	b2Body* groundBody = m_world->CreateBody(&groundBodyDef);
	// Make the ground fixture
	b2PolygonShape groundBox;
	groundBox.SetAsBox(50.0f, 10.0f);
	groundBody->CreateFixture(&groundBox, 0.0f);

	// Load the texture
	m_texture = Tengine::ResourceManager::getTexture("Assets/bricks_top.png");

	m_debugRenderer.init();

	// Test to add many boxes
	std::mt19937 randGenerator;
	std::uniform_real_distribution<float> xPos(-10.0f, 10.0f);
	std::uniform_real_distribution<float> yPos(-10.0f, 15.0f);
	std::uniform_real_distribution<float> size(0.5f, 2.5f);
	std::uniform_int_distribution<int> color(0, 255);
	const int NUM_BOXES = 10;

	for (int i = 0; i < NUM_BOXES; ++i)
	{
		Tengine::ColorRGBA8 randColor;
		randColor.r = color(randGenerator);
		randColor.g = color(randGenerator);
		randColor.b = color(randGenerator);
		randColor.a = 255;
		Box newBox;
		newBox.init(m_world.get(), glm::vec2(xPos(randGenerator), yPos(randGenerator)), glm::vec2(size(randGenerator), size(randGenerator)), m_texture, randColor, false);
		m_boxes.push_back(newBox);
	}

	//Initialize spritebatch
	m_spriteBatch.init();

	// Shader init
	// Compile the texture shader
	m_textureProgram.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
	m_textureProgram.addAttribute("vertexPosition");
	m_textureProgram.addAttribute("vertexColor");
	m_textureProgram.addAttribute("vertexUV");
	m_textureProgram.linkShaders();
	// Compile the light shader
	m_lightProgram.compileShaders("Shaders/lightShading.vert", "Shaders/lightShading.frag");
	m_lightProgram.addAttribute("vertexPosition");
	m_lightProgram.addAttribute("vertexColor");
	m_lightProgram.addAttribute("vertexUV");
	m_lightProgram.linkShaders();

	// Init camera
	m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
	m_camera.setScale(32.0f);

	// Init player
	m_player.init(m_world.get(), glm::vec2(0.0f, 30.0f), glm::vec2(2.0f), glm::vec2(1.0f, 1.8f), Tengine::ColorRGBA8(255, 255, 255, 255));

	initUI();
}

void GameplayScreen::onExit()
{
	m_debugRenderer.dispose();
}

void GameplayScreen::update()
{
	m_camera.update();
	checkInput();
	m_player.update(m_game->inputManager);

	// Update the physics simulation
	m_world->Step(1.0f / 60.0f, 6, 2);

}
void GameplayScreen::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	m_textureProgram.use();

	// Upload texture uniform
	GLint textureUniform = m_textureProgram.getUniformLocation("mySampler");
	glUniform1i(textureUniform, 0);
	glActiveTexture(GL_TEXTURE0);

	// Camera matrix
	glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
	GLint pUniform = m_textureProgram.getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	m_spriteBatch.begin();

	// Draw all the boxes
	for (auto& b : m_boxes)
	{
		b.draw(m_spriteBatch);
	}

	// Draw the player
	m_player.draw(m_spriteBatch);

	m_spriteBatch.end();
	m_spriteBatch.renderBatch();
	m_textureProgram.unuse();

	
	// Debug rendering
	if (m_renderDebug)
	{
		glm::vec4 destRect;
		for (auto& b : m_boxes)
		{
			destRect.x = b.getBody()->GetPosition().x - b.getDimensions().x / 2.0f;
			destRect.y = b.getBody()->GetPosition().y - b.getDimensions().y / 2.0f;
			destRect.z = b.getDimensions().x;
			destRect.w = b.getDimensions().y;

			m_debugRenderer.drawBox(destRect, Tengine::ColorRGBA8(255, 255, 255, 255), b.getBody()->GetAngle());
		}
		
		m_player.drawDebug(m_debugRenderer);

		
		// Render player
		m_debugRenderer.end();
		m_debugRenderer.render(projectionMatrix, 2.0f);
	}

	if (m_lightsOn)
	{
		// Render some test lights
		Light playerLight;
		playerLight.color = Tengine::ColorRGBA8(255, 255, 255, 128);
		playerLight.position = m_player.getPosition();
		playerLight.size = 30.0f;

		Light mouseLight;
		mouseLight.color = Tengine::ColorRGBA8(255, 0, 255, 150);
		mouseLight.position = m_camera.convertScreenToWorld(m_game->inputManager.getMouseCoords());
		mouseLight.size = 45.0f;

		m_lightProgram.use();
		pUniform = m_lightProgram.getUniformLocation("P");
		glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

		// Additive blending
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		m_spriteBatch.begin();

		playerLight.draw(m_spriteBatch);
		mouseLight.draw(m_spriteBatch);

		m_spriteBatch.end();
		m_spriteBatch.renderBatch();
		m_lightProgram.unuse();

		// Reset to regular alpha blending
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	m_gui.draw();
	
}

void GameplayScreen::initUI()
{
	// Initialize the UI
	m_gui.init("GUI");
	m_gui.loadScheme("TaharezLook.scheme");
	m_gui.setFont("DejaVuSans-10");
	// Add a test button
	CEGUI::PushButton* testButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.5f, 0.5f, 0.1f, 0.05f), glm::vec4(0.0f), "TestButton"));
	testButton->setText("Exit Game!");

	// Set the event to be called when we click
	testButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameplayScreen::onExitClicked, this));

	CEGUI::Combobox* testBComboBox = static_cast<CEGUI::Combobox*>(m_gui.createWidget("TaharezLook/Combobox", glm::vec4(0.2f, 0.2f, 0.1f, 0.05f), glm::vec4(0.0f), "TestCombobox"));


	m_gui.setMouseCursor("TaharezLook/MouseArrow");
	m_gui.showMouseCursor();
	SDL_ShowCursor(0);
}

void GameplayScreen::checkInput()
{
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt))
	{
		m_game->onSDLEvent(evnt);
		m_gui.onSDLEvent(evnt);
	}
}

bool GameplayScreen::onExitClicked(const CEGUI::EventArgs& e)
{
	m_currentState = Tengine::ScreenState::EXIT_APPLICATION;

	return true;
}

