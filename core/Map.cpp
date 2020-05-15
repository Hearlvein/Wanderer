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

	m_tilesPropertiesGrid.resize(GRID_WIDTH);
	m_tilesTypeGrid.resize(GRID_WIDTH);
	for (int i = 0; i < GRID_WIDTH; ++i)
	{
		m_tilesPropertiesGrid[i].resize(GRID_HEIGHT);
		m_tilesTypeGrid[i].resize(GRID_HEIGHT);
	}

	for (int i = 0; i < GRID_HEIGHT; ++i)
	{
		for (int j = 0; j < GRID_WIDTH; ++j)
		{
			int c;
			file >> c;
			m_tilesTypeGrid[j][i] = (TileType)c;

			if 		(m_tilesTypeGrid[j][i] == VoidTile)	  m_tilesPropertiesGrid[j][i] = Void;
			else if (m_tilesTypeGrid[j][i] == SolidTile1) m_tilesPropertiesGrid[j][i] = Solid;
			else if (m_tilesTypeGrid[j][i] == SolidTile2) m_tilesPropertiesGrid[j][i] = Solid;
			else if (m_tilesTypeGrid[j][i] == LadderTile) m_tilesPropertiesGrid[j][i] = Ladder;
		}
	}

	std::cout << GRID_WIDTH << "x" << GRID_HEIGHT << std::endl;

	for (int i = 0; i < GRID_WIDTH; ++i)
	{
		for (int j = 0; j < GRID_HEIGHT; ++j)
		{
			int tileNumber = static_cast<int>(m_tilesTypeGrid[i][j]);

			if (getTileProperty(i, j) != Void)
			{
				m_vertices.append({ {i * TILE_SIZE, j * TILE_SIZE}, 				{tileNumber * TILE_SIZE, 0.f} });
				m_vertices.append({ {(i + 1) * TILE_SIZE, j * TILE_SIZE}, 			{(tileNumber + 1) * TILE_SIZE, 0.f} });
				m_vertices.append({ {(i + 1) * TILE_SIZE, (j + 1) * TILE_SIZE},		{(tileNumber + 1) * TILE_SIZE, TILE_SIZE } });
				m_vertices.append({ {i * TILE_SIZE, (j + 1) * TILE_SIZE}, 			{tileNumber * TILE_SIZE, TILE_SIZE} });
			}
		}
	}

	std::cout << "grid size: " << GRID_WIDTH * GRID_HEIGHT << std::endl;
	std::cout << "vertex array size: " << m_vertices.getVertexCount() << std::endl;

	// Printing the grid in the console
	/*for (int i = 0; i < GRID_HEIGHT; ++i)
	{
		for (int j = 0; j < GRID_WIDTH; ++j)
			std::cout << (int)m_tilesPropertiesGrid[j][i] << " ";
		std::cout << std::endl;
	}*/
}

TileProperty Map::getTileProperty(int x, int y) const
{
	if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT)
		return m_tilesPropertiesGrid[x][y];
	else
	{
		// std::cout << "undef ref at " << x << ";" << y << std::endl;
		return Void;
	}
}

bool Map::touchingTile(const Box& box, TileProperty tileProperty) const
{
	int x_min = (int)std::floor(box.x / TILE_SIZE);
	int y_min = (int)std::floor(box.y / TILE_SIZE);
	int x_max = (int)std::ceil((box.x + box.w - 1) / TILE_SIZE);
	int y_max = (int)std::ceil((box.y + box.h - 1) / TILE_SIZE);

	for (int i = x_min; i < x_max; ++i)
		for (int j = y_min; j < y_max; ++j)
			if (getTileProperty(i, j) == tileProperty)
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
