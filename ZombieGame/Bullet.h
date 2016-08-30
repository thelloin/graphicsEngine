#pragma once

#include <glm/glm.hpp>
#include <Tengine/SpriteBatch.h>
#include <vector>

class Agent;
class Human;
class Zombie;

const int BULLET_RADIUS = 5;

class Bullet
{
public:
	Bullet(glm::vec2 position, glm::vec2 direction, float damage, float speed);
	~Bullet();

	// When return true, delete bullet
	bool update(const std::vector<std::string>& levelData);

	void draw(Tengine::SpriteBatch& spriteBatch);

	bool collideWithAgent(Agent* agent);

	// Getters
	float getDamage() const { return _damage; }

private:

	bool collideWithWorld(const std::vector<std::string>& levelData);

	glm::vec2 _position;
	glm::vec2 _direction;
	float _damage;
	float _speed;

};