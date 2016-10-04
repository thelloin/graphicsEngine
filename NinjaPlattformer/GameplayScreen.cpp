#include "GameplayScreen.h"

#include <Tengine/IMainGame.h>
#include <Tengine/ResourceManager.h>
#include <SDL/SDL.h>
#include <iostream>

#define SCREEN_INDEX_NO_SCREEN -1

GameplayScreen::GameplayScreen(Tengine::Window* window) : m_window(window)
{

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
	return SCREEN_INDEX_NO_SCREEN;
}

void GameplayScreen::build()
{

}

void GameplayScreen::destroy()
{

}

void GameplayScreen::onEntry()
{
	b2Vec2 gravity(0.0f, -9.81);
	m_world = std::make_unique<b2World>(gravity);

	// Make the ground
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -10.0f);
	b2Body* groundBody = m_world->CreateBody(&groundBodyDef);
	// Make the ground fixture
	b2PolygonShape groundBox;
	groundBox.SetAsBox(50.0f, 10.0f);
	groundBody->CreateFixture(&groundBox, 0.0f);

	Box newBox;
	newBox.init(m_world.get(), glm::vec2(0.0f, 14.0f), glm::vec2(15.0f, 15.0f));
	m_boxes.push_back(newBox);

	//Initialize spritebatch
	m_spriteBatch.init();

	// Shader init
	// Compile our color shader
	m_textureProgram.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
	m_textureProgram.addAttribute("vertexPosition");
	m_textureProgram.addAttribute("vertexColor");
	m_textureProgram.addAttribute("vertexUV");
	m_textureProgram.linkShaders();

	// Load the texture
	m_texture = Tengine::ResourceManager::getTexture("Assets/bricks_top.png");

	// Init camera
	m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
	m_camera.setScale(32.0f);
}

void GameplayScreen::onExit()
{

}

void GameplayScreen::update()
{
	std::cout << "Update\n";
	checkInput();
}
void GameplayScreen::draw()
{
	std::cout << "Draw\n";
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

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
		glm::vec4 destRect;
		destRect.x = b.getBody()->GetPosition().x;
		destRect.y = b.getBody()->GetPosition().y;
		destRect.z = b.getDimensions().x;
		destRect.w = b.getDimensions().y;
		m_spriteBatch.draw(destRect, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture.id, 0.0f, Tengine::ColorRGBA8(255, 0, 0, 255), b.getBody()->GetAngle());
	}

	m_spriteBatch.end();
	m_spriteBatch.renderBatch();

	m_textureProgram.unuse();
}

void GameplayScreen::checkInput()
{
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt))
	{
		m_game->onSDLEvent(evnt);
	}
}