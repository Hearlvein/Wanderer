#pragma once

#include "MovingGameObject.hpp"

class Fireball : public MovingGameObject
{
public:
	Fireball();
	virtual ~Fireball();

	virtual void update(float dt);
	void setDirection(const Direction& direction);

private:
	Direction m_direction;
};
