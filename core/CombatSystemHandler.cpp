#include "CombatSystemHandler.hpp"
#include "CombatState.hpp"
#include "AttackingState.hpp"

void CombatSystemHandler::update(float dt)
{
    for (auto ch1 : m_champions)
    {
        for (auto ch2 : m_champions)
        {
            if (&ch1 == &ch2)   // the two characters are the same
                continue;

            if (ch2->m_combatState == CombatState::Attacking &&
                ch2->m_attackingState == AttackingState::End &&
                boxesOverlapping(ch1->getHitbox(), ch2->getCurrentWeapon().hitbox) &&
                ch2->m_attackSucceed == false) //Checking if is hit by other characters
            {
                ch1->takeDamage(ch2->getCurrentWeapon().damage);
                ch2->m_attackSucceed = true;
            }

            if (ch1->m_combatState == CombatState::Attacking  &&
                ch1->m_attackingState == AttackingState::Parry &&
                boxesOverlapping(ch1->getCurrentWeapon().hitbox, ch2->getCurrentWeapon().hitbox) &&
                ch2->m_combatState == CombatState::Blocking) //checking if is blocked by other characters
            {
               ch1->open();
            }
        }
    }
}
