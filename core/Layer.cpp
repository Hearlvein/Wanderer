#include "Layer.hpp"

Layer::Layer()
{

}

Layer::~Layer()
{

}

void Layer::clear()
{
	m_drawables.clear();
}

void Layer::addObject(const sf::Drawable* drawable)
{
	m_drawables.push_back(drawable);
}

void Layer::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
	for (const sf::Drawable* drawable : m_drawables)
		rt.draw(*drawable, states);
}
