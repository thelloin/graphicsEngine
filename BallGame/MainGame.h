#pragma once
#include <vector>
#include <memory>

#include "Ball.h"
#include "Grid.h"
#include "BallRenderer.h"

enum class GameState { RUNNING, EXIT };

const int CELL_SIZE = 12;

class MainGame
{
public:
	MainGame();
	~MainGame();

	void run();
private:
	void init();
	void initRenderers();
	void initBalls();
	void update(float deltaTime);
	void draw();
	void drawHud();
	void processInput();

	int m_screenWidth = 0;
	int m_screenHeight = 0;

	std::vector<Ball> m_balls; ///< All the balls
	std::unique_ptr<Grid> m_grid; ///< Grid for spatial partitioning for collision

	int m_currentRenderer = 0;
	std::vector<std::unique_ptr<BallRenderer> > m_ballRenderers;
	
};