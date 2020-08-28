#pragma once

#include "Entity/Champion.hpp"
#include "Constants.hpp"

class Player : public Champion
{
public:
	Player()
	{
		AnimatedGameObject::loadFromFile(animationsPath + "player.txt");
		AnimatedGameObject::setCurrentAnimationName("right");
	}

	virtual ~Player() {}

	// virtual void update(float dt) override; // no need to overwrite for the moment
};
