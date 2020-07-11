#pragma once

#include "Champion.hpp"
#include <vector>

class CombatSystemHandler
{
public:
    CombatSystemHandler() {}
    ~CombatSystemHandler() {}

    void addCharacter(Champion* ch) { m_champions.push_back(ch); }
    void update(float dt);

private:
    std::vector<Champion*> m_champions;
};