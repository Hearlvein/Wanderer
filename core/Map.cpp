#include <iostream>
#include <cassert>
#include <fstream>
#include <cmath>
#include "Map.hpp"
#include "Constants.hpp"
#include "debug.hpp"

void Map::load(const std::string& filename,
    const std::function<void(const std::string& name, const sf::Vector2f& position)>& placeEntity)
{
    std::ifstream file(filename);
    std::string line;

    // used in nested loops
    std::size_t i = 0;
    std::size_t currentRowIndex = -1;

    // checks for file corruption. -1 if not initalized. otherwise value shouldn't change below
    int lineSize = -1;

    while (std::getline(file, line))
    {
        ++currentRowIndex;  // counts number of rows

        for (i = 0; i < line.size(); ++i)   // trim whitespaces
        {
            if (line[i] == ' ')
                line.erase(i, 1);
        }

        if (lineSize == -1) // set up GRID_WIDTH automatically from file
        {
            lineSize = line.size(); // new size, after trimming whitespaces
            GRID_WIDTH = lineSize;
            m_grid.resize(GRID_WIDTH);
        }

        // runtime assertion fails if level file is not properly set up
        assert((unsigned)lineSize == line.size());

        // reads file data and sets up m_grid procedurally
        for (i = 0; i < line.size(); ++i)
        {
            m_grid[i].reserve(currentRowIndex);

            char c = line[i];
            if (c == 'p')
            {
                placeEntity("player", { (float)i * TILE_SIZE, (float)(currentRowIndex - 1) * TILE_SIZE });
                c = '.';
            }
            else if (c == 'e')
            {
                placeEntity("enemy", { (float)i * TILE_SIZE, (float)(currentRowIndex - 1) * TILE_SIZE });
                c = '.';
            }

            m_grid[i].push_back(Tile::getTileFromIndex(c)); // note that c could be modified above
        }
    }

    // At this state, currentRowIndex represents the maximum index, so the vertical size-1 of the grid
    GRID_HEIGHT = currentRowIndex + 1;  // set up GRID_HEIGHT automatically from file

    std::cout << GRID_WIDTH << "x" << GRID_HEIGHT << std::endl;

    // creates an optimized (smaller than m_grid) vertex array
    regenerateVertices();

    std::cout << "grid size: " << GRID_WIDTH * GRID_HEIGHT << std::endl;
    std::cout << "vertex array size: " << m_vertices.size() << std::endl;

    // Printing the grid in the console
    /*for (int i = 0; i < GRID_HEIGHT; ++i)
    {
        for (int j = 0; j < GRID_WIDTH; ++j)
            std::cout << Tile::getPropertyCharIndexFromTile(m_grid[j][i]) << " ";
        std::cout << "\n";
    }*/
}

void Map::setTile(int x, int y, const Tile& tile)
{
    if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT)
    {
        Tile& gTile = m_grid[x][y];

        // testing property first for optimization
        if (gTile.property == tile.property && gTile.texCoords == tile.texCoords)   // same tile
            return;

        gTile = tile;
        regenerateVertices();

        std::cout << "vertex array size: " << m_vertices.size() << std::endl;
    }
    else
        std::cerr << "setTileProperty: undef ref at " << x << ";" << y << std::endl;
}

TileProperty Map::getTileProperty(int x, int y) const
{
    if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT)
        return m_grid[x][y].property;
    else
    {
        //std::cout << "getTileProperty: undef ref at " << x << ";" << y << std::endl;
        return Void;
    }
}

bool Map::touchingTile(const Box& box, TileProperty tileProperty) const
{
    // Hypothetical values
    int x_min = (int)std::floor(box.x / TILE_SIZE);
    int y_min = (int)std::floor(box.y / TILE_SIZE);
    int x_max = (int)std::ceil((box.x + box.w - 1) / TILE_SIZE);
    int y_max = (int)std::ceil((box.y + box.h - 1) / TILE_SIZE);

    // Smarting values
    if (x_min < 0)              x_min = 0;
    if (y_min < 0)              y_min = 0;
    if (x_max > GRID_WIDTH)     x_max = GRID_WIDTH;
    // commented to use virtual ground
    //if (y_max > GRID_HEIGHT)  y_max = GRID_HEIGHT;

    for (int i = x_min; i < x_max; ++i)
    {
        for (int j = y_min; j < y_max; ++j)
        {
            if (getTileProperty(i, j) == tileProperty)
                return true;
        }
    }

    if (y_max >= m_virtualGround)   // make the player unable to fall forever
    {
        std::cout << "Keeping the entity on a virtual ground" << std::endl;
        // @return true
        return true;
    }

    return false;
}

sf::Vector2f Map::getWorldSize() const
{
    return sf::Vector2f(static_cast<float>(GRID_WIDTH), static_cast<float>(GRID_HEIGHT)) * TILE_SIZE;
}

void Map::regenerateVertices()
{
    m_vertices.clear();

    // creates an optimized (smaller than m_grid) vertex array
    for (int i = 0; i < GRID_WIDTH; ++i)
    {
        for (int j = 0; j < GRID_HEIGHT; ++j)
        {
            if (getTileProperty(i, j) != Void)  // reduce vertices number
            {
                // set position & texture coords
                sf::Vector2f& texCoords = m_grid[i][j].texCoords;   // for read-ability

                m_vertices.emplace_back(sf::Vector2f( i * TILE_SIZE,       j * TILE_SIZE     ), texCoords                                                     );
                m_vertices.emplace_back(sf::Vector2f((i + 1) * TILE_SIZE,  j * TILE_SIZE     ), sf::Vector2f(texCoords.x + TILE_SIZE, texCoords.y            ));
                m_vertices.emplace_back(sf::Vector2f((i + 1) * TILE_SIZE, (j + 1) * TILE_SIZE), sf::Vector2f(texCoords.x + TILE_SIZE, texCoords.y + TILE_SIZE));
                m_vertices.emplace_back(sf::Vector2f( i * TILE_SIZE,      (j + 1) * TILE_SIZE), sf::Vector2f(texCoords.x,             texCoords.y + TILE_SIZE));
            }
        }
    }
}

void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.texture = m_texture;
    if (!m_vertices.empty())
        target.draw(&m_vertices[0], m_vertices.size(), sf::Quads, states);
}
