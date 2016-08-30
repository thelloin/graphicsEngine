#pragma once

#include <string>
#include <vector>

#include <Tengine/SpriteBatch.h>

const int TILE_WIDTH = 64;

class Level
{
public:
	// Load the level
	Level(const std::string& filename);
	~Level();

	void draw();

	// Getters
	int getWidth() const { return _levelData[0].size(); }
	int getHeight() const { return _levelData.size(); }
	int getNumHumans() const { return _numHumans; }
	glm::vec2 getStartPlayerPos() const { return _startPlayerPos;  }
	const std::vector<glm::ivec2>& getZombiesStartPos() const { return _zombieStartPos; }
	const std::vector<std::string>& getLevelData() const { return _levelData; }

private:
	std::vector<std::string> _levelData;
	int _numHumans;
	Tengine::SpriteBatch _spriteBatch;

	glm::vec2 _startPlayerPos;
	std::vector<glm::ivec2> _zombieStartPos;
};

