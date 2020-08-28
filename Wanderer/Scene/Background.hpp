#pragma once

#include <SFML/Graphics.hpp>

class Background
{
public:
	Background() {}
	~Background() {}

	void move(const sf::Vector2f& levelOffset)
	{
		m_backgroundSprite.move(levelOffset * m_speedFactor);
	}

	void resetPosition()
	{
		m_backgroundSprite.setPosition(0.f, 0.f);
	}

	const sf::Drawable* getDrawable() const { return &m_backgroundSprite; }
	sf::Sprite* getSprite() { return &m_backgroundSprite; }

private:
	float m_speedFactor = 0.5f;
	sf::Sprite m_backgroundSprite;
};
