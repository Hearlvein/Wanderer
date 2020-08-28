#pragma once

#include <iostream>
#include <string>
#include "Utility/Box.hpp"

struct Weapon
{
    std::string name;
    Box hitbox;
    int damage;
    float attackTime;
};