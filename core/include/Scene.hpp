#pragma once

#include <SFML/Graphics.hpp>

class Scene
{
public:
	Scene(sf::RenderWindow* window);
	virtual ~Scene();

	virtual void handleEvent(const sf::Event& event) = 0;
	virtual void checkInput() = 0;
	virtual void update(float dt) = 0;
	virtual void draw(sf::RenderTarget& target) = 0;

protected:
	sf::RenderWindow* m_window;
};

