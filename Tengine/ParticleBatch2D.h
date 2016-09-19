#pragma once
#include <glm/glm.hpp>
#include "Vertex.h"
#include "SpriteBatch.h"
#include "GLTexture.h"

namespace Tengine {

	class Particle2D {
	public:
		friend class ParticleBatch2D;

		void update(float deltaTime);

	private:
		glm::vec2 m_position = glm::vec2(0.0f);
		glm::vec2 m_velocity = glm::vec2(0.0f);
		ColorRGBA8 m_color;
		float m_life = 0.0;
		float m_width = 0.0f;
	};

	class ParticleBatch2D
	{
	public:
		ParticleBatch2D();
		~ParticleBatch2D();

		void init(int maxParticles,
				  float decayRate,
				  GLTexture texture);

		void update(float deltaTime);

		void draw(SpriteBatch* spriteBatch);

		void AddParticle(const glm::vec2& position,
						 const glm::vec2& velocity,
						 const ColorRGBA8& color,
						 float width);

	private:
		int findFreeParticle();

		float m_decayRate = 0.1f;
		Particle2D* m_particles = nullptr;
		int m_maxParticles = 0;
		int m_lastFreeParticle = 0;
		GLTexture m_texture;
	};

}