#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.hpp"

class BackgroundManager
{
public:
	BackgroundManager();
	~BackgroundManager();

	void update(const sf::Vector2f& levelOffset);
	const sf::Drawable* getDrawable() const;

private:
	float m_speedFactor = 0.5f;
	sf::Texture m_backgroundTexture;
	sf::Sprite m_backgroundSprite;
};