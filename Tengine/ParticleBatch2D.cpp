#include "ParticleBatch2D.h"

namespace Tengine {

	void Particle2D::update(float deltaTime)
	{
		m_position += m_velocity * deltaTime;
	}

	ParticleBatch2D::ParticleBatch2D()
	{
		// Empty
	}


	ParticleBatch2D::~ParticleBatch2D()
	{
		delete[] m_particles;
	}

	void ParticleBatch2D::init(int maxParticles,
							   float decayRate,
							   GLTexture texture)
	{
		m_maxParticles = maxParticles;
		m_particles = new Particle2D[maxParticles];
		m_decayRate = decayRate;
		m_texture = texture;
	}

	void ParticleBatch2D::update(float deltaTime)
	{
		for (int i = 0; i < m_maxParticles; ++i)
		{
			// Check if particle is active
			if (m_particles[i].m_life > 0.0)
			{
				m_particles[i].update(deltaTime);
				m_particles[i].m_life -= m_decayRate * deltaTime;
			}
		}
	}

	void ParticleBatch2D::draw(SpriteBatch* spriteBatch)
	{
		glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
		for (int i = 0; i < m_maxParticles; ++i)
		{
			// Check if particle is active
			auto& p = m_particles[i];
			if (p.m_life > 0.0)
			{
				glm::vec4 destRect(p.m_position.x, p.m_position.y, p.m_width, p.m_width);
				spriteBatch->draw(destRect, uvRect, m_texture.id, 0.0f, p.m_color);
			}
		}
	}

	void ParticleBatch2D::AddParticle(const glm::vec2& position,
									  const glm::vec2& velocity,
									  const ColorRGBA8& color,
									  float width)
	{
		int particleIndex = findFreeParticle();

		auto& p = m_particles[particleIndex];
		p.m_life = 1.0f;
		p.m_position = position;
		p.m_velocity = velocity;
		p.m_color = color;
		p.m_width = width;
	}

	int ParticleBatch2D::findFreeParticle()
	{
		for (int i = m_lastFreeParticle; i < m_maxParticles; ++i)
		{
			if (m_particles[i].m_life <= 0.0f)
			{
				m_lastFreeParticle = i;
				return i;
			}
		}

		for (int i = 0; i < m_lastFreeParticle; ++i)
		{
			if (m_particles[i].m_life <= 0.0f)
			{
				m_lastFreeParticle = i;
				return i;
			}
		}

		// No particle are free, overrite first particle
		return 0;
	}

}