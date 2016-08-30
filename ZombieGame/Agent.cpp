#include "Agent.h"
#include <Tengine/ResourceManager.h>
#include "Level.h"
#include <algorithm>

#include <iostream>



Agent::Agent()
{
}


Agent::~Agent()
{
}

bool Agent::collideWithLevel(const std::vector<std::string>& levelData) 
{
	std::vector<glm::vec2> collideTilePositions;

	// Check the four corners
	// First corner
	checkTilePosition(levelData, collideTilePositions, _position.x, _position.y);
	// Second corner
	checkTilePosition(levelData, collideTilePositions, _position.x + TILE_WIDTH, _position.y);
	// Third corner
	checkTilePosition(levelData, collideTilePositions, _position.x, _position.y + TILE_WIDTH);
	// Fourth corner
	checkTilePosition(levelData, collideTilePositions, _position.x + TILE_WIDTH, _position.y + TILE_WIDTH);
	
	if (collideTilePositions.size() == 0) {
		return false;
	}

	// Do the collision
	for (int i = 0; i < collideTilePositions.size(); i++) {
		collideWithTile(collideTilePositions[i]);
	}

	return true;
}

bool Agent::collideWithAgent(Agent* agent)
{
	const float MIN_DISTANCE = AGENT_RADIUS * 2.0f;

	glm::vec2 centerPosA = _position + glm::vec2(AGENT_RADIUS);
	glm::vec2 centerPosB = agent->getPosition() + glm::vec2(AGENT_RADIUS);

	glm::vec2 distVec = centerPosA - centerPosB;

	float distance = glm::length(distVec);

	float collisionDepth = MIN_DISTANCE - distance;
	if (collisionDepth > 0) {

		glm::vec2 collisionDepthVec = glm::normalize(distVec) * collisionDepth;

		_position += collisionDepthVec / 2.0f;
		agent->_position -= collisionDepthVec / 2.0f;
		return true;
	}

	return false;
}

void Agent::draw(Tengine::SpriteBatch& _spriteBatch) {

	static int textureID = Tengine::ResourceManager::getTexture("Textures/circle.png").id;

	const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

	glm::vec4 destRect;
	destRect.x = _position.x;
	destRect.y = _position.y;
	destRect.z = AGENT_WIDTH;
	destRect.w = AGENT_WIDTH;

	_spriteBatch.draw(destRect, uvRect, textureID, 0.0f, _color);
}

bool Agent::applyDamage(float damage)
{
	_health -= damage;
	if (_health <= 0) {
		return true;
	}
	
	return false;
}

void Agent::checkTilePosition(const std::vector<std::string>& levelData,
							  std::vector<glm::vec2>& collideTilePositions,
							  float x,
							  float y) {

	// Get the position of this corner in grid-space
	glm::vec2 gridPos = glm::vec2(floor(x / (float)TILE_WIDTH),
									floor(y / (float)TILE_WIDTH));

	// We are outside the world, just return
	if (gridPos.x < 0 || gridPos.x >= levelData[0].size() ||
		gridPos.y < 0 || gridPos.y >= levelData.size()) {
		return;
	}

	
	// If this is not an air tile, we should collide with it
	if (levelData[gridPos.y][gridPos.x] != '.') {
		collideTilePositions.push_back(gridPos * (float)TILE_WIDTH + glm::vec2((float)TILE_WIDTH / 2.0f));
	}
}

// AABB (Axis Aligned Bouding Box) collision
void Agent::collideWithTile(glm::vec2 tilePos) {
	/* TODO A problem is that you can get stuck with this current implementation,
	* a solution is to sort the colliding tiles and choose to correct the one
	* with the greatest distance.
	*/
	
	const float TILE_RADIUS = (float)TILE_WIDTH / 2.0f;
	const float MIN_DISTANCE = AGENT_RADIUS + TILE_RADIUS;
	
	glm::vec2 centerAgentPos = _position + glm::vec2(AGENT_RADIUS);
	glm::vec2 distVec = centerAgentPos - tilePos;

	// Get the depth of the collision
	float xDepth = MIN_DISTANCE - abs(distVec.x);
	float yDepth = MIN_DISTANCE - abs(distVec.y);

	// If either of the depth are > 0, then we collided
	if (xDepth > 0 && yDepth > 0) {
		if (std::max(xDepth,0.0f) < std::max(yDepth, 0.0f)) {
			if (distVec.x <= 0) {
				_position.x -= xDepth;
			}
			else {
				_position.x += xDepth;
			}
		}
		else {
			if (distVec.y <= 0) {
				_position.y -= yDepth;
			}
			else {
				_position.y += yDepth;
			}
		}
	}
}
