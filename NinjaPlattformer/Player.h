#pragma once

#include "Capsule.h"
#include <Tengine/SpriteBatch.h>
#include <Tengine/TileSheet.h>
#include <Tengine/InputManager.h>
#include <Tengine/TileSheet.h>
#include <Tengine/DebugRenderer.h>

enum class PlayerMoveState {STANDING, RUNNING, PUNCHING, IN_AIR};

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

	glm::vec2 getPosition() const {
		glm::vec2 rv;
		rv.x = m_capsule.getBody()->GetPosition().x;
		rv.y = m_capsule.getBody()->GetPosition().y;
		return rv;
	}

private:
	glm::vec2 m_drawDims;
	Tengine::ColorRGBA8 m_color;
	Tengine::TileSheet m_texture;
	Capsule m_capsule;
	PlayerMoveState m_moveState = PlayerMoveState::STANDING;
	float m_animTime = 0.0f;
	int m_direction = 1; // 1 or -1
	bool m_onGround = false;
	bool m_isPunching = false;
};