#include "MovingGameObject.hpp"

#include <iostream>
#include <cmath>

MovingGameObject::MovingGameObject()
{
	setYState(Falling);
	// std::cout << "moving entity constructor" << std::endl;
}

MovingGameObject::~MovingGameObject()
{

}

void MovingGameObject::update(float dt)
{
	// YState update
	if (m_yState == Jumping && m_timeJumping >= m_maxTimeJumping)
    {
        setYState(Falling);
        //std::cout << "reseting jump" << std::endl;
    }

    // WalkingState update
    m_timeWalkingState += dt;
    if (m_walkingState == Beginning && m_timeWalkingState >= m_maxTimeWalkingBeginning)
    {
        setWalkingState(Middle);
        // std::cout << "changing to middle state" << std::endl;
    }
    else if (m_walkingState == End && m_timeWalkingState >= m_maxTimeWalkingEnd)
    {
        setWalkingState(Idle);
        // std::cout << "definitely stopping" << std::endl;
    }

    // Calculate y movement
    m_movement = sf::Vector2f();    // Reset

	if (m_yState == Falling)
	{
		m_timeFalling += dt;
		m_velocity.y = m_alphaFalling * m_timeFalling;
		m_movement.y = m_velocity.y * dt;
		//std::cout << "[FALLING] : movement.y=" << m_movement.y << std::endl;
	}
	else if (m_yState == Jumping)
	{
		m_timeJumping += dt;
		m_velocity.y = -m_alphaJumping * m_timeJumping + m_maxVelocityYJumping;
		m_movement.y = -m_velocity.y * dt;
		//std::cout << "[JUMPING] : movement.y=" << m_movement.y << std::endl;
	}
	else if (m_yState == Climbing)
	{
		if (m_climbingDirection == Down)
			m_movement.y = m_climbingVelocity * dt;
		else if (m_climbingDirection == Up)
			m_movement.y = -m_climbingVelocity * dt;
		else if (m_climbingDirection == None)
			m_movement.y = 0.f;
	}

	// Calculate x movement
	// std::cout << "m_walkingState=" << m_walkingState << std::endl;
	if (m_walkingState != Idle)
	{
	    if (m_walkingState == Beginning)
            m_velocity.x = m_alphaBeginning * m_timeWalkingState;
        else if (m_walkingState == Middle)
        	m_velocity.x = m_maxVelocityX;
        else	// m_walkingState == End
            m_velocity.x = -m_alphaEnd * m_timeWalkingState + m_maxVelocityX;

        if (m_facing == Left)
        	m_velocity.x *= -1;

        // std::cout << "velx=" << m_velocity.x << std::endl;

        m_movement.x = m_velocity.x * dt;
	}

	m_movement.x = std::round(m_movement.x);
	m_movement.y = std::round(m_movement.y);
}

// Getters
const Direction& MovingGameObject::getFacing() const
{
	return m_facing;
}

const sf::Vector2f& MovingGameObject::getVelocity() const
{
	return m_velocity;
}

const sf::Vector2f& MovingGameObject::getMovement() const
{
	return m_movement;
}

const WalkingState& MovingGameObject::getWalkingState() const
{
	return m_walkingState;
}

const yState& MovingGameObject::getYState() const
{
	return m_yState;
}

// Setters
void MovingGameObject::setFacing(const Direction& facing)
{
	m_facing = facing;
}

void MovingGameObject::setVelocity(float x, float y)
{
    m_velocity = sf::Vector2f(x, y);
}

void MovingGameObject::setWalkingState(const WalkingState& walkingState)
{
	if (    walkingState == m_walkingState 							    	||
		(   walkingState == Beginning 		&& m_walkingState == Middle)    ||
		(   walkingState == End 			&& m_walkingState == Idle)
	) return;

	m_walkingState = walkingState;
    m_timeWalkingState = 0.f;

    if (m_walkingState == Idle)
        m_velocity.x = 0.f;
}

void MovingGameObject::setYState(const yState& state)
{
	if (    state == m_yState 						||
		(   state == Jumping && m_yState != Grounded)
	) return;

	m_yState = state;
	m_velocity.y = 0.f;

	if (m_yState == Falling)
		m_timeFalling = 0.f;
	else if (m_yState == Jumping)
		m_timeJumping = 0.f;
}

void MovingGameObject::setClimbingDirection(const Direction& direction)
{
	m_climbingDirection = direction;
}
