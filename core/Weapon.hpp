#pragma once

#include <iostream>
#include <string>
#include "Box.hpp"

struct Weapon
{
    std::string name;
    Box hitbox;
    int damage;
    float attackTime;
};