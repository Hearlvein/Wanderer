#include "Enemy.hpp"
#include "Constants.hpp"

Enemy::Enemy()
{
	AnimatedGameObject::loadFromFile(animationsPath + "enemy.txt");
	AnimatedGameObject::setCurrentAnimationName("right");
	setWalkingState(WalkingState::Beginning);

	MovingCharacter::m_maxVelocityX = 200.0f;
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