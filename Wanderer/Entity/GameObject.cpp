#include "Entity/GameObject.hpp"
#include "Utility/debug.hpp"

GameObject::GameObject()
{
	std::cout << "Creating a GameObject (" << this << ")" << std::endl;
}

GameObject::~GameObject()
{
	std::cout << "Destroying  a GameObject (" << this << ")" << std::endl;
}

const sf::Vector2f& GameObject::getPosition() const
{
	return m_sprite.getPosition();
}

void GameObject::setTextureRect(const sf::IntRect& textureRect, const Box* newHitbox)
{
	m_sprite.setTextureRect(textureRect);

	// !: m_currentHitbox is absolute and newHitbox is not
	if (newHitbox)
	{
		m_currentHitbox.x = getPosition().x + newHitbox->x;
		m_currentHitbox.y = getPosition().y + newHitbox->y;
		m_currentHitbox.w = newHitbox->w;
		m_currentHitbox.h = newHitbox->h;
	}
	else
	{
		/*m_currentHitbox.x = getPosition().x;
		m_currentHitbox.y = getPosition().y;*/
		m_currentHitbox.w = static_cast<float>(textureRect.width);
		m_currentHitbox.h = static_cast<float>(textureRect.height);
	}
}

const sf::IntRect& GameObject::getTextureRect() const
{
	return m_sprite.getTextureRect();
}

void GameObject::setTexture(const sf::Texture& texture)
{
	m_sprite.setTexture(texture);
}

const Box& GameObject::getHitbox() const
{
	return m_currentHitbox;
}

void GameObject::setPosition(float x, float y)
{
	m_currentHitbox.x = x;
	m_currentHitbox.y = y;

	m_sprite.setPosition(x, y);
}

void GameObject::setPosition(const sf::Vector2f& pos)
{
	setPosition(pos.x, pos.y);
}

void GameObject::move(const sf::Vector2f& offset)
{
	setPosition(getPosition() + offset);
}

void GameObject::setSize(float x, float y)
{
	m_currentHitbox.w = x;
	m_currentHitbox.h = y;
}

void GameObject::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_sprite, states);
}
