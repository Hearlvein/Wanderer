#pragma once

#include "Utility/Box.hpp"

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class MapEditor;

/** The class GameObject represents in-game solids **/
class GameObject : public sf::Drawable
{
public:
	GameObject();
	virtual ~GameObject();

	virtual void update(float dt) = 0;
	
	void setTexture(const sf::Texture& texture);
	void setTextureRect(const sf::IntRect& textureRect, const Box* newHitbox = nullptr);

	const sf::Vector2f& getPosition() const;
	const sf::IntRect& getTextureRect() const;
	virtual const Box& getHitbox() const;

private:
	friend class GameScene;
	friend class MapEditor;

	sf::Sprite m_sprite;

	void setPosition(float x, float y);
	void setPosition(const sf::Vector2f& pos);
	void move(const sf::Vector2f& offset);
	void setSize(float x, float y);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	Box m_currentHitbox;
};
