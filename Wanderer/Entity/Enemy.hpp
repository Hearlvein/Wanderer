#pragma once

#include "Entity/MovingCharacter.hpp"

class Enemy : public MovingCharacter
{
public:
	Enemy();
	virtual ~Enemy();

	void toggleFacing();
};

/*
après, tu pleurs
*/