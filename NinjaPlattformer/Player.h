#pragma once

#include "Capsule.h"
#include <Tengine/SpriteBatch.h>
#include <Tengine/GLTexture.h>
#include <Tengine/InputManager.h>
#include <Tengine/TileSheet.h>
#include <Tengine/DebugRenderer.h>

class Player
{
public:

	void init(b2World* world, 
			  const glm::vec2& position,
			  const glm::vec2& drawDims, 
			  glm::vec2& collisionDims,
			  Tengine::ColorRGBA8 color);

	void draw(Tengine::SpriteBatch& spriteBatch);
	void drawDebug(Tengine::DebugRenderer& debugRenderer);

	void update(Tengine::InputManager& inputManager);

	const Capsule& getBox() const { return m_capsule; }

private:
	glm::vec2 m_drawDims;
	Tengine::ColorRGBA8 m_color;
	Tengine::GLTexture m_texture;
	Capsule m_capsule;
};