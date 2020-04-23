#pragma once

#include "TileType.hpp"

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Map : public sf::Drawable
{
public:
	Map(const sf::Texture* texture);
	virtual ~Map();

	void load(const std::string& filename);

	// Read-only
	const std::vector<TileType>& operator[](int n) const;
	// Read-write
	std::vector<TileType>& operator[](int n);

	bool isEmptyArea(float x, float y, float w, float h) const;
	bool touchingTile(float x, float y, float w, float h, const TileType& tileType) const;
	sf::Vector2f getWorldSize() const;

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	const sf::Texture* m_texture = nullptr;
	int GRID_WIDTH = 0;
	int GRID_HEIGHT = 0;
	sf::VertexArray m_vertices;
	std::vector< std::vector<TileType> > m_grid;
};
