#include <iostream>
#include <fstream>
#include <cmath>
#include "Map.hpp"
#include "Constants.hpp"

Map::Map(const sf::Texture* texture) :
	m_texture(texture)
{
	m_vertices.setPrimitiveType(sf::Quads);
}

Map::~Map()
{

}

void Map::load(const std::string& filename)
{
	std::ifstream file(filename.c_str());
	file >> GRID_WIDTH;
	file >> GRID_HEIGHT;

	m_grid.resize(GRID_WIDTH);
	for (int i = 0; i < GRID_WIDTH; ++i)
		m_grid[i].resize(GRID_HEIGHT);

	for (int i = 0; i < GRID_HEIGHT; ++i)
	{
		for (int j = 0; j < GRID_WIDTH; ++j)
		{
			int c;
			file >> c;
			m_grid[j][i] = (TileType)c;
		}
	}

	std::cout << GRID_WIDTH << "x" << GRID_HEIGHT << std::endl;

	int ignored = 0;

	for (int i = 0; i < GRID_WIDTH; ++i)
	{
		for (int j = 0; j < GRID_HEIGHT; ++j)
		{
			int tileNumber = (int)m_grid[i][j];

			if (tileNumber != 0)
			{
				m_vertices.append(sf::Vertex(sf::Vector2f(i * TILE_SIZE, j * TILE_SIZE), 				sf::Vector2f(tileNumber * TILE_SIZE, 0.f)));
				m_vertices.append(sf::Vertex(sf::Vector2f((i + 1) * TILE_SIZE, j * TILE_SIZE), 			sf::Vector2f((tileNumber + 1) * TILE_SIZE, 0.f)));
				m_vertices.append(sf::Vertex(sf::Vector2f((i + 1) * TILE_SIZE, (j + 1) * TILE_SIZE),	sf::Vector2f((tileNumber + 1) * TILE_SIZE, TILE_SIZE)));
				m_vertices.append(sf::Vertex(sf::Vector2f(i * TILE_SIZE, (j + 1) * TILE_SIZE), 			sf::Vector2f(tileNumber * TILE_SIZE, TILE_SIZE)));
			}
		}
	}

	std::cout << "total ignored: " << ignored << std::endl;
	std::cout << "grid size: " << GRID_WIDTH * GRID_HEIGHT << std::endl;
	std::cout << "vertex array size: " << m_vertices.getVertexCount() << std::endl;

	// Printing the grid in the console
	/*for (int i = 0; i < GRID_HEIGHT; ++i)
	{
		for (int j = 0; j < GRID_WIDTH; ++j)
			std::cout << m_grid[j][i] << " ";
		std::cout << std::endl;
	}*/
}

// Read-only
const std::vector<TileType>& Map::operator[](int n) const
{
    return m_grid[n];
}

// Read-write
std::vector<TileType>& Map::operator[](int n)
{
	return m_grid[n];
}

bool Map::isEmptyArea(float x, float y, float w, float h) const
{
	if (x < 0.f || y < 0.f || (x + w) > GRID_WIDTH * TILE_SIZE || (y + h) > GRID_HEIGHT * TILE_SIZE)
		return false;

	int x_min = (int)std::floor(x / TILE_SIZE);
	int y_min = (int)std::floor(y / TILE_SIZE);
	int x_max = (int)std::ceil((x + w - 1) / TILE_SIZE);
	int y_max = (int)std::ceil((y + h - 1) / TILE_SIZE);

	for (int i = x_min; i < x_max; ++i)
		for (int j = y_min; j < y_max; ++j)
			if (m_grid[i][j] == SolidBlock)
			{
				//std::cout << "(" << i << ";" << j << ") returned is SolidBlock!" << std::endl;
				return false;
			}

	return true;
}

bool Map::touchingTile(float x, float y, float w, float h, const TileType& tileType) const
{
	if (x < 0.f || y < 0.f || (x + w) > GRID_WIDTH * TILE_SIZE || (y + h) > GRID_HEIGHT * TILE_SIZE)
		return false;

	int x_min = (int)std::floor(x / TILE_SIZE);
	int y_min = (int)std::floor(y / TILE_SIZE);
	int x_max = (int)std::ceil((x + w - 1) / TILE_SIZE);
	int y_max = (int)std::ceil((y + h - 1) / TILE_SIZE);

	for (int i = x_min; i < x_max; ++i)
		for (int j = y_min; j < y_max; ++j)
			if (m_grid[i][j] == tileType)
			{
				//std::cout << "(" << i << ";" << j << ") returned is SolidBlock!" << std::endl;
				return true;
			}

	return false;
}

sf::Vector2f Map::getWorldSize() const
{
	return sf::Vector2f(static_cast<float>(GRID_WIDTH), static_cast<float>(GRID_HEIGHT)) * TILE_SIZE;
}

void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.texture = m_texture;
    target.draw(m_vertices, states);
}
