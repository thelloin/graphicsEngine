#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <Tengine/AudioEngine.h>

#include "Bullet.h"

class Gun
{
public:
	Gun(std::string name, int fireRate, int bulletsPerShot, float spread,
		float bulletDamage, float bulletSpeed, Tengine::SoundEffect fireEffect);
	~Gun();

	void update(bool isMouseDown,
				const glm::vec2& position,
				const glm::vec2& direction,
				std::vector<Bullet>& bullets,
				float deltaTime);

private:
	void fire(const glm::vec2& direction,
			  const glm::vec2& position,
			  std::vector<Bullet>& bullets);

	Tengine::SoundEffect m_fireEffect;

	std::string _name;

	int _fireRate; ///< Fire rate in terms of frames

	int _bulletsPerShot; ///< How many bullets are fired at a time

	float _spread; ///< Accurscy

	float _bulletSpeed;
	float _bulletDamage;

	float _frameCounter; ///< Count frames so we know when to shoot bullets
};

