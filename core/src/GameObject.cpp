#include "GameObject.hpp"

GameObject::GameObject()
{

}

GameObject::~GameObject()
{

}

const sf::Vector2f& GameObject::getPosition() const
{
	return m_sprite.getPosition();
}

void GameObject::setTextureRect(const sf::IntRect& textureRect, const sf::Vector2f* newHitboxSize)
{
	m_sprite.setTextureRect(textureRect);

	if (newHitboxSize)
	{
		m_currentHitbox->w = newHitboxSize->x;
		m_currentHitbox->h = newHitboxSize->y;
	}
	else
	{
		m_currentHitbox->w = static_cast<float>(textureRect.width);
		m_currentHitbox->h = static_cast<float>(textureRect.height);
	}
}

void GameObject::setTexture(const sf::Texture& texture)
{
	m_sprite.setTexture(texture);
}

const Box& GameObject::getCurrentHitbox() const
{
	return *m_currentHitbox;
}

void GameObject::setPosition(float x, float y)
{
	m_currentHitbox->x = x;
	m_currentHitbox->y = y;

	m_sprite.setPosition(x, y);
}

void GameObject::setSize(float x, float y)
{
	m_currentHitbox->w = x;
	m_currentHitbox->h = y;
}

void GameObject::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_sprite, states);
}
