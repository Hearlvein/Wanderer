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

const sf::Vector2f& GameObject::getSize() const
{
	return m_size;
}

const sf::Vector2i GameObject::getTexturePosition() const
{
	return sf::Vector2i(m_sprite.getTextureRect().left, m_sprite.getTextureRect().top);
}

void GameObject::setTexturePosition(const sf::Vector2i& position)
{
	m_sprite.setTextureRect(sf::IntRect(position.x, position.y, static_cast<int>(m_size.x), static_cast<int>(m_size.y)));
}

void GameObject::setTexture(const sf::Texture& texture)
{
	m_sprite.setTexture(texture);
}

void GameObject::setPosition(float x, float y)
{
	m_sprite.setPosition(x, y);
}

void GameObject::setSize(float x, float y)
{
	m_size = sf::Vector2f(x, y);
}

void GameObject::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_sprite, states);
}
