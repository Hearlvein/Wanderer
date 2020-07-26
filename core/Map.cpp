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
    std::ifstream file(filename.c_str());
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

    // Create default m_ptrGridVertices (see .hpp for meaning of this variable)
    m_ptrGridVertices.reserve(GRID_WIDTH);
    for (int i = 0; i < GRID_WIDTH ; ++i)
        m_ptrGridVertices.emplace_back(std::vector<std::size_t>(GRID_HEIGHT, 0));   // default value should never be accessed

    // creates an optimized (smaller than m_grid) vertex array
    for (int i = 0; i < GRID_WIDTH; ++i)
    {
        for (int j = 0; j < GRID_HEIGHT; ++j)
        {
            if (getTileProperty(i, j) != Void)  // reduce vertices number
            {
                m_vertices.append({ {i * TILE_SIZE, j * TILE_SIZE},                 m_grid[i][j].texturePosition });
                // Here the index
                m_ptrGridVertices[i][j] = m_vertices.getVertexCount() - 1;

                m_vertices.append({ {(i + 1) * TILE_SIZE, j * TILE_SIZE},           {m_grid[i][j].texturePosition.x + TILE_SIZE, m_grid[i][j].texturePosition.y} });
                m_vertices.append({ {(i + 1) * TILE_SIZE, (j + 1) * TILE_SIZE},     {m_grid[i][j].texturePosition.x + TILE_SIZE, m_grid[i][j].texturePosition.y + TILE_SIZE} });
                m_vertices.append({ {i * TILE_SIZE, (j + 1) * TILE_SIZE},           {m_grid[i][j].texturePosition.x, m_grid[i][j].texturePosition.y + TILE_SIZE} });
            }
        }
    }

    std::cout << "grid size: " << GRID_WIDTH * GRID_HEIGHT << std::endl;
    std::cout << "vertex array size: " << m_vertices.getVertexCount() << std::endl;

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
        // update texture
        if (getTileProperty(x, y) != Void)
        {
            sf::Vertex* quad = &m_vertices[ m_ptrGridVertices[x][y] ];
            quad[0].texCoords = tile.texturePosition;
            quad[1].texCoords = { tile.texturePosition.x + TILE_SIZE, tile.texturePosition.y};
            quad[2].texCoords = { tile.texturePosition.x + TILE_SIZE, tile.texturePosition.y + TILE_SIZE };
            quad[3].texCoords = { tile.texturePosition.x,             tile.texturePosition.y + TILE_SIZE};
        }
        else
        {
            m_vertices.append({ {x * TILE_SIZE, y * TILE_SIZE},                 tile.texturePosition });
            // Here the index
            m_ptrGridVertices[x][y] = m_vertices.getVertexCount() - 1;

            m_vertices.append({ {(x + 1) * TILE_SIZE, y * TILE_SIZE},           {tile.texturePosition.x + TILE_SIZE, tile.texturePosition.y} });
            m_vertices.append({ {(x + 1) * TILE_SIZE, (y + 1) * TILE_SIZE},     {tile.texturePosition.x + TILE_SIZE, tile.texturePosition.y + TILE_SIZE} });
            m_vertices.append({ {x * TILE_SIZE, (y + 1) * TILE_SIZE},           {tile.texturePosition.x, tile.texturePosition.y + TILE_SIZE} });
        }

        m_grid[x][y] = tile;
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

void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.texture = m_texture;
    target.draw(m_vertices, states);
}
