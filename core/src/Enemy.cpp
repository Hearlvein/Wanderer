#include "Enemy.hpp"

Enemy::Enemy()
{

}

Enemy::~Enemy()
{

}

void Enemy::toggleFacing()
{
	if (m_facing == Right)
		m_facing = Left;
	else if (m_facing == Left)
		m_facing = Right;
}