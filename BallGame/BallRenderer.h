#pragma once

#include <vector>
#include <memory>
#include <Tengine/SpriteBatch.h>
#include <Tengine/GLSLProgram.h>
#include "Ball.h"

// Ball renderer interface
class BallRenderer
{
public:
	virtual void renderBalls(Tengine::SpriteBatch& spriteBatch,
							 const std::vector<Ball>& balls,
							 const glm::mat4& projectionMatrix);
protected:
	std::unique_ptr<Tengine::GLSLProgram> m_program = nullptr;
};

// Visualizes kinetic energy
class MomentumBallRenderer : public BallRenderer
{
public:
	virtual void renderBalls(Tengine::SpriteBatch& spriteBatch,
							 const std::vector<Ball>& balls,
							 const glm::mat4& projectionMatrix) override;
};

/*
 * This Renderer is not implemented yet
// Visualizes positive X component of velocity, as well as position
class VelocityBallRenderer : public BallRenderer
{
public:
	VelocityBallRenderer(int screenWidth, int screenHeight);

	virtual void renderBalls(Tengine::SpriteBatch& spriteBatch,
							 const std::vector<Ball>& balls,
							 const glm::mat4& projectionMatrix) override;
private:
	int m_screenWidth;
	int m_screenHeight;
};
*/

