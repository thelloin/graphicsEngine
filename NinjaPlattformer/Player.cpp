#include "Player.h"
#include <Tengine/ResourceManager.h>
#include <SDL/SDL.h>

#include <iostream>

Player::Player()
{

}

Player::~Player()
{

}

void Player::init(b2World* world, const glm::vec2& position, glm::vec2& dimensions, Tengine::ColorRGBA8 color)
{
	// Load the texture
	Tengine::GLTexture texture = Tengine::ResourceManager::getTexture("Assets/blue_ninja.png");

	m_collisionBox.init(world, position, dimensions, texture, color, true, glm::vec4(0.0f, 0.0f, 0.1f, 0.5f));
}

void Player::draw(Tengine::SpriteBatch& spriteBatch)
{
	m_collisionBox.draw(spriteBatch);
}

void Player::update(Tengine::InputManager& inputManager)
{
	b2Body* body = m_collisionBox.getBody();

	if (inputManager.isKeyDown(SDLK_a))
	{
		body->ApplyForceToCenter(b2Vec2(-100.0, 0.0), true);
	}
	else if (inputManager.isKeyDown(SDLK_d))
	{
		body->ApplyForceToCenter(b2Vec2(100.0, 0.0), true);
	}
	else
	{
		// Apply damping
		body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x * 0.95f, body->GetLinearVelocity().y));
	}

	const float MAX_SPEED = 10.0f;
	if (body->GetLinearVelocity().x < -MAX_SPEED)
	{
		body->SetLinearVelocity(b2Vec2(-MAX_SPEED, body->GetLinearVelocity().y));
	}
	else if (body->GetLinearVelocity().x > MAX_SPEED)
	{
		body->SetLinearVelocity(b2Vec2(MAX_SPEED, body->GetLinearVelocity().y));
	}

	// Loop through all the contact points of the player to
	// determine if he is standing on the ground
	for (b2ContactEdge* ce = body->GetContactList(); ce != nullptr; ce = ce->next)
	{
		b2Contact* c = ce->contact;
		if (c->IsTouching())
		{
			b2WorldManifold manifold;
			c->GetWorldManifold(&manifold);
			// Check if the points are below
			bool below = false;
			for (int i = 0; i < b2_maxManifoldPoints; i++)
			{
				if (manifold.points[i].y < body->GetPosition().y - m_collisionBox.getDimensions().y / 2.0f + 0.01f)
				{
					below = true;
					break;
				}
			}
			if (below) // He can jump
			{
				if (inputManager.isKeyPressed(SDLK_w)) // Move up this if??
				{
					body->ApplyLinearImpulse(b2Vec2(0.0, 30.0), b2Vec2(0.0, 0.0), true);
					break;
				}
			}
		}
	}
}