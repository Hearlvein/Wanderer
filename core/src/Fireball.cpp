#include "Fireball.hpp"
#include <iostream>
#include <cmath>

Fireball::Fireball()
{

}
Fireball::~Fireball()
{

}


void Fireball::update(float dt)
{
	m_movement.x = m_velocity.x * dt * ((m_direction == Left) ? -1 : 1);

	m_movement.x = std::round(m_movement.x);
	std::cout << "after update : dx=" << m_movement.x << std::endl;
	m_movement.y = 0.f;
}

void Fireball::setDirection(const Direction& direction)
{
	m_direction = direction;
}
