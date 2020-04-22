#pragma once

#include <SFML/Graphics.hpp>

class GameObject : public sf::Drawable
{
public:
	GameObject();
	virtual ~GameObject();

	virtual void update(float dt) = 0;
	const sf::Vector2f& getPosition() const;
	const sf::Vector2f& getSize() const;
	const sf::Vector2i getTexturePosition() const;
	void setTexturePosition(const sf::Vector2i& position);
	void setTexture(const sf::Texture& texture);

private:
	friend class GameScene;

	void setPosition(float x, float y);
	void setSize(float x, float y);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	sf::Sprite m_sprite;
    sf::Vector2f m_size = { 50.f, 100.f };
};
