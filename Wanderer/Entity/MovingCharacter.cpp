#include "Entity/MovingCharacter.hpp"

#include <iostream>
#include <cmath>

MovingCharacter::MovingCharacter()
{
	setYState(YState::Falling);
	//std::cout << "moving character ctor" << std::endl;
}

MovingCharacter::~MovingCharacter()
{
	//std::cout << "moving character dtor" << std::endl;
}

void MovingCharacter::update(float dt)
{
	// YState update
	if (m_yState == YState::Jumping && m_timeJumping >= m_maxTimeJumping)
	{
		setYState(YState::Falling);
		//std::cout << "reseting jump" << std::endl;
	}

	// WalkingState update
	m_timeWalkingState += dt;
	if (m_walkingState == WalkingState::Beginning && m_timeWalkingState >= m_maxTimeWalkingBeginning)
	{
		setWalkingState(WalkingState::Middle);
		// std::cout << "changing to middle state" << std::endl;
	}
	else if (m_walkingState == WalkingState::End && m_timeWalkingState >= m_maxTimeWalkingEnd)
	{
		setWalkingState(WalkingState::Idle);
		// std::cout << "definitely stopping" << std::endl;
	}

	// Calculate y movement
	m_movement = sf::Vector2f();    // Reset

	if (m_yState == YState::Falling)
	{
		m_timeFalling += dt;
		m_velocity.y = m_alphaFalling * m_timeFalling;
		m_movement.y = m_velocity.y * dt;
		//std::cout << "[FALLING] : movement.y=" << m_movement.y << std::endl;
	}
	else if (m_yState == YState::Jumping)
	{
		m_timeJumping += dt;
		m_velocity.y = -m_alphaJumping * m_timeJumping + m_maxVelocityYJumping;
		m_movement.y = -m_velocity.y * dt;
		//std::cout << "[JUMPING] : movement.y=" << m_movement.y << std::endl;
	}
	else if (m_yState == YState::Climbing)
	{
		if (m_climbingDirection == Direction::Down)
			m_movement.y = m_climbingVelocity * dt;
		else if (m_climbingDirection == Direction::Up)
			m_movement.y = -m_climbingVelocity * dt;
		else if (m_climbingDirection == Direction::None)
			m_movement.y = 0.f;
	}

	// Calculate x movement
	// std::cout << "m_walkingState=" << m_walkingState << std::endl;
	if (m_walkingState != WalkingState::Idle)
	{
		if (m_walkingState == WalkingState::Beginning)
			m_velocity.x = m_alphaBeginning * m_timeWalkingState;
		else if (m_walkingState == WalkingState::Middle)
			m_velocity.x = m_maxVelocityX;
		else	// m_walkingState == End
			m_velocity.x = -m_alphaEnd * m_timeWalkingState + m_maxVelocityX;

		if (m_facing == Direction::Left)
			m_velocity.x *= -1;

		// std::cout << "velx=" << m_velocity.x << std::endl;

		m_movement.x = m_velocity.x * dt;
	}

	m_movement.x = std::round(m_movement.x);
	m_movement.y = std::round(m_movement.y);

	updateAnimation();
	Character::update(dt);
}

void MovingCharacter::updateAnimation()
{
	if (m_yState == YState::Climbing)
		AnimatedGameObject::setCurrentAnimationName("climbing");
	else
	{
		if (m_facing == Direction::Right)
		{
			if (m_walkingState == WalkingState::Idle)
				AnimatedGameObject::setCurrentAnimationName("idleRight");
			else
				AnimatedGameObject::setCurrentAnimationName("right");
		}
		else
		{
			if (m_walkingState == WalkingState::Idle)
				AnimatedGameObject::setCurrentAnimationName("idleLeft");
			else
				AnimatedGameObject::setCurrentAnimationName("left");
		}
	}
}

// Getters
const Direction& MovingCharacter::getFacing() const
{
	return m_facing;
}

const sf::Vector2f& MovingCharacter::getVelocity() const
{
	return m_velocity;
}

const sf::Vector2f& MovingCharacter::getMovement() const
{
	return m_movement;
}

WalkingState MovingCharacter::getWalkingState() const
{
	return m_walkingState;
}

YState MovingCharacter::getYState() const
{
	return m_yState;
}

// Setters
void MovingCharacter::setFacing(const Direction& facing)
{
	m_facing = facing;
}

void MovingCharacter::setVelocity(float x, float y)
{
	m_velocity = sf::Vector2f(x, y);
}

void MovingCharacter::setWalkingState(WalkingState walkingState)
{
	if (walkingState == m_walkingState ||
		(walkingState == WalkingState::Beginning && m_walkingState == WalkingState::Middle) ||
		(walkingState == WalkingState::End && m_walkingState == WalkingState::Idle)
		) return;

	m_walkingState = walkingState;
	m_timeWalkingState = 0.f;

	if (m_walkingState == WalkingState::Idle)
		m_velocity.x = 0.f;
}

void MovingCharacter::setYState(YState state)
{
	if (state == m_yState)	// No change to perform
		return;

	// can't jump mid-air
	if (state == YState::Jumping && (m_yState == YState::Jumping || m_yState == YState::Falling))
		return;

	m_yState = state;
	m_velocity.y = 0.f;

	if (m_yState == YState::Falling)
		m_timeFalling = 0.f;
	else if (m_yState == YState::Jumping)
		m_timeJumping = 0.f;
}

void MovingCharacter::setClimbingDirection(const Direction& direction)
{
	m_climbingDirection = direction;
}
