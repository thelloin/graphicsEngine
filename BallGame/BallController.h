#pragma once

#include <vector>

#include "Ball.h"

enum class GravityDirection { NONE, LEFT, UP, RIGHT, DOWN };

class Grid;

class BallController
{
public:
	/// Update the balls
	void updateBalls(std::vector<Ball>& balls, Grid* grid, float deltaTime, int maxX, int maxY);
	/// Some event functions
	void onMouseDown(std::vector<Ball>& balls, float mouseX, float mouseY);
	void onMouseUp(std::vector<Ball>& balls);
	void onMouseMove(std::vector<Ball>& balls, float mouseX, float mouseY);
	void setGravityDirection(GravityDirection dir) { m_gravityDirection = dir; }
private:
	/// Update collision
	void updateCollision(Grid* grid);
	///Checks collision between a ball and a vector of balls, starting at a specific index
	void checkCollision(Ball* ball, std::vector<Ball*>& ballsToCheck, int startingIndex);
	/// Checks collision between two balls
	void checkCollision(Ball& ball1, Ball& ball2);
	/// Returns true if the mouse is hovering over a ball
	bool isMouseOnBall(Ball& ball, float mouseX, float mouseY);
	glm::vec2 getGravityAccel();

	int m_grabbedBall = -1; ///< The ball we are currently grabbing on to
	glm::vec2 m_prevPos = glm::vec2(0.0f); ///< Previous position of the grabbed ball
	glm::vec2 m_grabOffset = glm::vec2(0.0f); ///< Offset of the cursor on the selected ball

	GravityDirection m_gravityDirection = GravityDirection::NONE;
};

