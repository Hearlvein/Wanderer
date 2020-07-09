#pragma once

#include "MovingCharacter.hpp"

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