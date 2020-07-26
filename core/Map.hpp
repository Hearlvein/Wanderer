#pragma once

#include "Tile.hpp"
#include "Box.hpp"

#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>
#include <string>

class Map : public sf::Drawable
{
public:
	Map() { m_vertices.setPrimitiveType(sf::Quads); }
	virtual ~Map() {}

	void setTexture(const sf::Texture& texture) { m_texture = &texture; }
	void load(const std::string& filename,
		const std::function<void(const std::string& name, const sf::Vector2f& position)>& placeEntity);

	void setTile(int x, int y, const Tile& tile);
	TileProperty getTileProperty(int x, int y) const;

	bool touchingTile(const Box& box, TileProperty tileProperty) const;
	sf::Vector2f getWorldSize() const;

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	const sf::Texture* m_texture = nullptr;
	int GRID_WIDTH = 0;
	int GRID_HEIGHT = 0;
	std::vector< std::vector<Tile> > m_grid;

	// Holds an index for every cell of the grid which represents their vertices (in m_vertices)
	// Used to modify m_vertices and add/delete new vertices 
	std::vector< std::vector<std::size_t> > m_ptrGridVertices;
	sf::VertexArray m_vertices;

	const int m_virtualGround = 60;	// player can't fall forever
};
