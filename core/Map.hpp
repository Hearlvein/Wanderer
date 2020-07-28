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
	Map() {}
	virtual ~Map() {}

	void setTexture(const sf::Texture& texture) { m_texture = &texture; }
	void load(const std::string& filename,
		const std::function<void(const std::string& name, const sf::Vector2f& position)>& placeEntity);

	void setTile(int x, int y, char index);
	char getTileIndex(int x, int y) const;
	const Tile& getTile(int x, int y) const;    // get tile from index
	TileProperty getTileProperty(int x, int y) const;

	bool touchingTile(const Box& box, TileProperty tileProperty) const;
	sf::Vector2f getWorldSize() const;

private:
	friend class MapEditor;
	
	/** heavy internal method called everytime grid is modified **/
	void regenerateVertices();
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	const sf::Texture* m_texture = nullptr;
	int GRID_WIDTH = 0;
	int GRID_HEIGHT = 0;
	std::vector< std::vector<char> > m_grid;	// nows only stores indexes (more precise)

	std::vector<sf::Vertex> m_vertices;

	int m_virtualGround;	// entities won't fall forever
};
