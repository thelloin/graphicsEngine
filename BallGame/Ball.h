#pragma once

#include <glm/glm.hpp>
#include <Tengine/Vertex.h>

struct Cell;

// POD
struct Ball
{
public:
	Ball(float radius, float mass, const glm::vec2& pos,
		 const glm::vec2& vel, unsigned int textrureId,
		 const Tengine::ColorRGBA8 color);

	float radius;
	float mass;
	glm::vec2 velocity;
	glm::vec2 position;
	unsigned int textureId = 0;
	Tengine::ColorRGBA8 color;
	Cell* ownerCell = nullptr;
	int cellVectorIndex = -1;
};

