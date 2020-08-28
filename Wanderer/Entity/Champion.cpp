#include "Entity/Champion.hpp"

#include "Constants.hpp"
#include "Utility/util.hpp"

void Champion::update(float dt)
{
	// Placing the weapon hitbox
	m_weapon.hitbox.y = getHitbox().y + (getHitbox().h - m_weapon.hitbox.h) / 2;
	if (m_facing == Direction::Right)
		m_weapon.hitbox.x = getHitbox().x + getHitbox().w;
	else
		m_weapon.hitbox.x = getHitbox().x - m_weapon.hitbox.w;
	
	// Handling combat state
	// converting seconds to milliseconds to keep floating type
	float elapsedTime = m_combatTimer.getElapsedTime().asSeconds() * 1000.f;
	switch(m_combatState)
	{
		case CombatState::Attacking:
			if (elapsedTime < m_weapon.attackTime / 4)
				setAttackingState(AttackingState::Begin);
			
			else if (elapsedTime >= m_weapon.attackTime / 4 && elapsedTime < m_weapon.attackTime * (2/4))
				setAttackingState(AttackingState::Mid);
			
			else if (elapsedTime >= m_weapon.attackTime * (2/4) && elapsedTime < m_weapon.attackTime * (3/4))
				setAttackingState(AttackingState::Parry);
			
			else if (elapsedTime >= m_weapon.attackTime * (3/4) && elapsedTime < m_weapon.attackTime)
				setAttackingState(AttackingState::End);
			
			else
				setCombatState(CombatState::Idle);

			break;

		case CombatState::Blocking:
			if (elapsedTime >= m_blockingTime)
				setCombatState(CombatState::Idle);
			break;
			
		case CombatState::Opened:
			if (elapsedTime >= m_openingTime)
				setCombatState(CombatState::Idle);
			break;

		// doing nothing for other values
		case CombatState::Idle:
		case CombatState::Dodging:
		default:
			break;
	}

	MovingCharacter::update(dt);
}

void Champion::attack()
{
	if (m_combatState == CombatState::Idle)
	{
		setCombatState(CombatState::Attacking);
		m_combatTimer.restart();
		m_attackSucceed = false;
	}
}

void Champion::block()
{
	if (m_combatState == CombatState::Idle)
	{
		setCombatState(CombatState::Blocking);
		m_combatTimer.restart();
	}
}

void Champion::dodge()
{
	setCombatState(CombatState::Dodging);	// calling the smart setter
}

void Champion::open()
{
	setCombatState(CombatState::Opened);
	m_combatTimer.restart();
}

void Champion::setCombatState(CombatState state)
{
	if (m_combatState == state)	// prevent from multiple useless calls
		return;

	m_combatState = state;
}

void Champion::setAttackingState(AttackingState state)
{
	if (m_attackingState == state)	// no change to perform then
		return;

	m_attackingState = state;
}

void Champion::cancelAttack()
{
	// only attacks in begin phase can be canceled
	if (m_combatState == CombatState::Attacking && m_attackingState == AttackingState::Begin)
		m_combatState = CombatState::Idle;
}
