#pragma once

#include "GameObject.hpp"
#include "yState.hpp"
#include "WalkingState.hpp"
#include "Direction.hpp"

class MovingEntity : public virtual GameObject
{
public:
	MovingEntity();
	virtual ~MovingEntity();

	virtual void update(float dt);

	// Getters
	const Direction& getFacing() const;
	const sf::Vector2f& getVelocity() const;
	const sf::Vector2f& getMovement() const;
	const WalkingState& getWalkingState() const;
	const yState& getYState() const;

	// Setters
	void setFacing(const Direction& facing);
	void setVelocity(float x, float y);
	void setWalkingState(const WalkingState& walkingState);
	void setYState(const yState& state);
	void setClimbingDirection(const Direction& direction);

protected:
	Direction m_facing = Right;
	sf::Vector2f m_velocity = { 0.f, 60.f };
	sf::Vector2f m_movement;

	WalkingState m_walkingState = Idle;
	float m_timeWalkingState;
	const float m_maxTimeWalkingBeginning = 0.3f;
	const float m_maxTimeWalkingEnd = 0.3f;
	const float m_maxVelocityX = 400.f;
	const float m_alphaBeginning = m_maxVelocityX / m_maxTimeWalkingBeginning;
	const float m_alphaEnd = m_maxVelocityX / m_maxTimeWalkingEnd;

	yState m_yState;
	float m_timeFalling;
	float m_timeJumping;
	const float m_maxTimeJumping = 0.3f;
	const float m_maxVelocityYJumping = 1300.f;
	const float m_alphaJumping = m_maxVelocityYJumping / m_maxTimeJumping;
	const float m_alphaFalling = m_alphaJumping;

	const float m_climbingVelocity = 200.f;
	Direction m_climbingDirection;
};
