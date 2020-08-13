#pragma once

#include "TilesManager.hpp"
#include "Tile.hpp"
#include "Box.hpp"

#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>
#include <string>

class Map : public sf::Drawable
{
public:
	Map(TilesManager& mt) : m_tilesMgr(mt) {}
	virtual ~Map() {}

	void printGrid();

	void setTexture(const sf::Texture& texture) { m_texture = &texture; }
	void load(const std::string& filename,
		const std::function<void(const std::string& name, const sf::Vector2f& position)>& placeEntity);

	void setTile(int x, int y, const Tile* newTile, bool* newColLeft = nullptr, bool* newRowTop = nullptr);

	void addColsRight	(std::size_t n);
	void addColsLeft	(std::size_t n);
	void addRowsBottom	(std::size_t n);
	void addRowsTop		(std::size_t n);

	void removeEmptyColsRight();
	bool removeEmptyColsLeft();
	void removeEmptyRowsBottom();
	bool removeEmptyRowsTop();

	char getTileIndex(int x, int y) const;
	const Tile* getTile(int x, int y) const;    // get tile from index
	Tile::Property getTileProperty(int x, int y) const;

	bool touchingTile(const Box& box, Tile::Property tileProperty);
	sf::Vector2f getWorldSize() const;

private:
	friend class MapEditor;

	/** heavy internal method called everytime grid is modified **/
	void regenerateVertices();
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	// Grid
	std::vector< std::vector<const Tile*> > m_grid;	// Pointers shouldn't be invalidated, nor be nullptr
	int GRID_WIDTH = 0;
	int GRID_HEIGHT = 0;
	const sf::Vector2i m_minimumGridSize { 3, 3 };

	// Tiles list
	TilesManager& m_tilesMgr;	// Helper that contains all posible tiles grid  refers to

	// Graphics
	const sf::Texture* m_texture = nullptr;
	std::vector<sf::Vertex> m_vertices;

	// Behavior
	int m_virtualGround;	// entities won't fall forever
};
