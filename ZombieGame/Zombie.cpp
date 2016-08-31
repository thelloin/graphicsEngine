#include "Zombie.h"

#include "Human.h"

Zombie::Zombie()
{
}


Zombie::~Zombie()
{
}

void Zombie::init(float speed, glm::vec2 pos) 
{
	_speed = speed;
	_position = pos;
	_health = 150;
	// Set green color
	_color = Tengine::ColorRGBA8(0, 160, 0, 255);
}

void Zombie::update(const std::vector<std::string>& levelData,
					std::vector<Human*>& humans,
					std::vector<Zombie*>& zombies,
					float deltaTime) 
{
	// Find the closest human
	Human* closestHuman = getNearestHuman(humans);

	// If we found a human, move towards him
	if (closestHuman != nullptr) {
		// Get the direction vector towards the human
		glm::vec2 direction = glm::normalize(closestHuman->getPosition() - _position);
		_position += direction * _speed * deltaTime;
	}

	// Do collision
	collideWithLevel(levelData);
}

Human* Zombie::getNearestHuman(std::vector<Human*>& humans) 
{
	Human* closestHuman = nullptr;
	float smallestDistance = 9999999.0f;

	for (int i = 0; i < humans.size(); i++) {
		// Get the direction vector
		glm::vec2 distVec = humans[i]->getPosition() - _position;
		// Get distance
		float distance = glm::length(distVec);

		// If this person is closer than the closest person, this is the new closest
		if (distance < smallestDistance) {
			smallestDistance = distance;
			closestHuman = humans[i];
		}
	}

	return closestHuman;
}
