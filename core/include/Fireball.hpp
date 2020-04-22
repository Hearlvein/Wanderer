#pragma once

#include "MovingEntity.hpp"

class Fireball : public MovingEntity
{
public:
	Fireball();
	virtual ~Fireball();

	virtual void update(float dt);
	void setDirection(const Direction& direction);

private:
	Direction m_direction;
};
