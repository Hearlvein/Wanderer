#pragma once

#include "Entity/MovingCharacter.hpp"
#include "Entity/CombatState.hpp"
#include "Entity/AttackingState.hpp"
#include "Entity/Weapon.hpp"

class Champion : public MovingCharacter
{
public:
	Champion() : MovingCharacter() {}
	virtual ~Champion() {}

	virtual void update(float dt) override;

	void attack();
	void open();
	void block();
	void dodge();
	void cancelAttack();
	void setCombatState(CombatState state);
	void setAttackingState(AttackingState state);

	const Weapon& getCurrentWeapon() const { return m_weapon; }

protected:

	friend class CombatSystemHandler;
	
	// time management
	sf::Clock m_combatTimer;
	float m_openingTime;
	float m_blockingTime;

	Weapon m_weapon;

	// states
	CombatState m_combatState;
	AttackingState m_attackingState;
	bool m_attackSucceed;
};