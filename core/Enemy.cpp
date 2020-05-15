#include "Enemy.hpp"

Enemy::Enemy()
{

}

Enemy::~Enemy()
{

}

void Enemy::toggleFacing()
{
	if (m_facing == Direction::Right)
		m_facing = Direction::Left;
	else if (m_facing == Direction::Left)
		m_facing = Direction::Right;
}