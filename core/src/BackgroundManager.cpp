#include "BackgroundManager.hpp"

BackgroundManager::BackgroundManager()
{
	m_backgroundTexture.loadFromFile(texturesPath + "background.png");
	m_backgroundSprite.setTexture(m_backgroundTexture);
}

BackgroundManager::~BackgroundManager()
{

}

void BackgroundManager::update(const sf::Vector2f& levelOffset)
{
	m_backgroundSprite.move(levelOffset * m_speedFactor);
}

const sf::Drawable* BackgroundManager::getDrawable() const
{
	return &m_backgroundSprite;
}