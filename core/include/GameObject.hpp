#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "Box.hpp"

/** The class GameObject represents in-game solids **/
class GameObject : public sf::Drawable
{
public:
	GameObject();
	virtual ~GameObject();

	virtual void update(float dt) = 0;
	const sf::Vector2f& getPosition() const;
	void setTextureRect(const sf::IntRect& textureRect, const sf::Vector2f* newHitboxSize = nullptr);
	void setTexture(const sf::Texture& texture);
	const Box& getCurrentHitbox() const;

private:
	friend class GameScene;

	sf::Sprite m_sprite;

	void setPosition(float x, float y);
	void setSize(float x, float y);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    std::unique_ptr<Box> m_currentHitbox;
};
