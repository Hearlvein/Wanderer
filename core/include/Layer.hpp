#pragma once

#include <SFML/Graphics.hpp>
#include <list>

class Layer : public sf::Drawable
{
public:
	Layer();
	virtual ~Layer();

	void addObject(const sf::Drawable* drawable);

private:
	virtual void draw(sf::RenderTarget& rt, sf::RenderStates states) const override;

	std::list<const sf::Drawable*> m_drawables;
};
