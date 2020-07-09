#pragma once

#include "GameObject.hpp"
#include "YState.hpp"
#include "WalkingState.hpp"
#include "Direction.hpp"

class MovingGameObject : public virtual GameObject
{
public:
	MovingGameObject() {}
	virtual ~MovingGameObject() {}

	virtual void update(float dt) = 0;
	const sf::Vector2f& getMovement() const { return m_movement; }

protected:
	sf::Vector2f m_movement;
};