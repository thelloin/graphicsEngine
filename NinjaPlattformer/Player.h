#pragma once
#include "Box.h"

#include <Tengine/SpriteBatch.h>
#include <Tengine/GLTexture.h>
#include <Tengine/InputManager.h>

class Player
{
public:
	Player();
	~Player();

	void init(b2World* world, const glm::vec2& position, glm::vec2& dimensions, Tengine::ColorRGBA8 color);

	void draw(Tengine::SpriteBatch& spriteBatch);

	void update(Tengine::InputManager& inputManager);

	const Box& getBox() const { return m_collisionBox; }

private:
	Box m_collisionBox;
};