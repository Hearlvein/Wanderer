#pragma once

#include "Character.hpp"

class Enemy : public Character
{
public:
	Enemy();
	virtual ~Enemy();

	void toggleFacing();
	
};

/*
après, tu pleurs
*/