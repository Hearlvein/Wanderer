#pragma once

#include "Tile.hpp"
#include "Box.hpp"

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Map : public sf::Drawable
{
public:
	Map(const sf::Texture* texture);
	virtual ~Map();

	void load(const std::string& filename);

	TileProperty getTileProperty(int x, int y) const;

	bool touchingTile(const Box& box, TileProperty tileProperty) const;
	sf::Vector2f getWorldSize() const;

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	const sf::Texture* m_texture = nullptr;
	int GRID_WIDTH = 0;
	int GRID_HEIGHT = 0;
	sf::VertexArray m_vertices;
	std::vector< std::vector<TileType> > m_tilesTypeGrid;
	std::vector< std::vector<TileProperty> > m_tilesPropertiesGrid;
};
