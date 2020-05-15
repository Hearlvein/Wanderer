#include "Constants.hpp"
#include "Character.hpp"
#include "util.hpp"

Character::Character()
{

}

Character::~Character()
{

}

void Character::update(float dt)
{
	if (m_isInvicible)
	{
		m_invicibilityTime += dt;
		if (m_invicibilityTime >= m_invicibilityMaxTime)
			setIsInvicible(false);
	}

	if (!isAlive())
		std::cout << "character died" << std::endl;

	MovingGameObject::update(dt);

	// Link bases
	if (m_yState == YState::Climbing)
		setCurrentSeq("climbing");
	else
	{
		if (m_facing == Direction::Right)
			if (m_walkingState == WalkingState::Idle)
				setCurrentSeq("idleRight");
			else
				setCurrentSeq("right");
		else
			if (m_walkingState == WalkingState::Idle)
				setCurrentSeq("idleLeft");
			else
				setCurrentSeq("left");
	}

	AnimatedGameObject::update(dt);
}

void Character::takeDamage(unsigned int amount)
{
	if (!m_isInvicible)
	{
		if (amount >= m_hp)
			m_hp = 0;
		else
			m_hp -= amount;
	}
}

bool Character::isInvicible() const
{
	return m_isInvicible;
}

void Character::setIsInvicible(bool isInvicible, float time)
{
	if (m_isInvicible == isInvicible || !isAlive())
		return;

	m_isInvicible = isInvicible;

	if (m_isInvicible)
	{
		m_invicibilityMaxTime = time;
		m_invicibilityTime = 0.f;
		std::cout << "invicible" << std::endl;
	}
	else
	{
		std::cout << "no more invicible" << std::endl;
	}
}

bool Character::isAlive() const
{
	return m_hp > 0;
}
