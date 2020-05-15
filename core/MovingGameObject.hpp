#pragma once

#include "GameObject.hpp"
#include "YState.hpp"
#include "WalkingState.hpp"
#include "Direction.hpp"

class MovingGameObject : public virtual GameObject
{
public:
	MovingGameObject();
	virtual ~MovingGameObject();

	virtual void update(float dt);

	// Getters
	const Direction& getFacing() const;
	const sf::Vector2f& getVelocity() const;
	const sf::Vector2f& getMovement() const;
	WalkingState getWalkingState() const;
	YState getYState() const;

	// Setters
	void setFacing(const Direction& facing);
	void setVelocity(float x, float y);
	void setWalkingState(WalkingState walkingState);
	void setYState(YState state);
	void setClimbingDirection(const Direction& direction);

protected:
	Direction m_facing = Direction::Right;
	sf::Vector2f m_velocity = { 0.f, 60.f };
	sf::Vector2f m_movement;

	WalkingState m_walkingState = WalkingState::Idle;
	float m_timeWalkingState;
	const float m_maxTimeWalkingBeginning = 0.3f;
	const float m_maxTimeWalkingEnd = 0.3f;
	const float m_maxVelocityX = 400.f;
	const float m_alphaBeginning = m_maxVelocityX / m_maxTimeWalkingBeginning;
	const float m_alphaEnd = m_maxVelocityX / m_maxTimeWalkingEnd;

	YState m_yState;
	float m_timeFalling;
	float m_timeJumping;
	const float m_maxTimeJumping = 0.3f;
	const float m_maxVelocityYJumping = 1300.f;
	const float m_alphaJumping = m_maxVelocityYJumping / m_maxTimeJumping;
	const float m_alphaFalling = m_alphaJumping;

	const float m_climbingVelocity = 200.f;
	Direction m_climbingDirection;
};
