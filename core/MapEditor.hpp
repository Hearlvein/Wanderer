#pragma once

#include "GameObject.hpp"
#include "Constants.hpp"
#include "Tile.hpp"
#include <SFML/Graphics.hpp>
#include <fstream>

class Map;

class MapEditor
{
public:
	MapEditor(sf::RenderWindow& mapWindow,
			  Map& map,
			  const std::map<char, const GameObject&>& entities,
			  const sf::Texture& texture)
		: m_mapWindow(mapWindow)
		, m_map(map)
		, m_entities(entities)
		, m_texture(texture)
	{
		m_tilesWindow.create({ m_tilesPerRow * (unsigned)TILE_SIZE, m_tilesPerCol * (unsigned)TILE_SIZE }, "Tiles", sf::Style::Titlebar);

		m_tilesSprites['a'] = sf::Sprite(m_texture, { (int)TILE_SIZE, 0, (int)TILE_SIZE, (int)TILE_SIZE });
		m_tilesSprites['a'].setPosition({ 0.f, 0.f });

		m_tilesSprites['b'] = sf::Sprite(m_texture, { 2 * (int)TILE_SIZE, 0, (int)TILE_SIZE, (int)TILE_SIZE });
		m_tilesSprites['b'].setPosition({ TILE_SIZE, 0.f });

		m_tilesSprites['l'] = sf::Sprite(m_texture, { 3 * (int)TILE_SIZE, 0, (int)TILE_SIZE, (int)TILE_SIZE });
		m_tilesSprites['l'].setPosition({ 2 * TILE_SIZE, 0.f });
	}

	~MapEditor() {}

	void render()
	{
		m_tilesWindow.clear();

		for (auto it = m_tilesSprites.begin(); it != m_tilesSprites.end(); ++it)
			m_tilesWindow.draw(it->second);

		m_tilesWindow.display();
	}

	void checkInput()
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && m_editTimer.getElapsedTime() > sf::milliseconds(100))
		{
			// in tiles window?
			sf::Vector2i mouseInTilesWindow = sf::Mouse::getPosition(m_tilesWindow);
			// std::cout << "mouseInTilesWindow: " << mouseInTilesWindow.x << ";" << mouseInTilesWindow.y << std::endl;

			if (mouseInTilesWindow.x > 0 && mouseInTilesWindow.x < (int)m_tilesWindow.getSize().x &&
				mouseInTilesWindow.y > 0 && mouseInTilesWindow.y < (int)m_tilesWindow.getSize().y)
			{
			    // std::cout << "in tiles window" << std::endl;

				for (auto& [i, sprite] : m_tilesSprites)
				{
					if (sprite.getGlobalBounds().contains((float)mouseInTilesWindow.x, (float)mouseInTilesWindow.y))
					{
						m_selectedCharIndex = i;
						// std::cout << "m_selectedCharIndex: " << m_selectedCharIndex << std::endl;
					}
				}

				return;
			}

			// in map window?
			sf::Vector2i mouseInMapWindow = sf::Mouse::getPosition(m_mapWindow);
			// std::cout << "mouseInMapWindow: " << mouseInMapWindow.x << ";" << mouseInMapWindow.y << std::endl;

			if (mouseInMapWindow.x > 0 && mouseInMapWindow.x < (int)m_mapWindow.getSize().x &&
				mouseInMapWindow.y > 0 && mouseInMapWindow.y < (int)m_mapWindow.getSize().y)
			{
				sf::Vector2f worldCoords = m_mapWindow.mapPixelToCoords(mouseInMapWindow);
				sf::Vector2i mouseTileCoords = {
					(int)(worldCoords.x / TILE_SIZE),
					(int)(worldCoords.y / TILE_SIZE)
				};
				// std::cout << "mouseTileCoords: " << mouseTileCoords.x << ";" << mouseTileCoords.y << std::endl;

				m_map.setTile(mouseTileCoords.x, mouseTileCoords.y, m_selectedCharIndex, true);
			}

			m_editTimer.restart();
		}
		else if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && m_editTimer.getElapsedTime() > sf::milliseconds(100))
        {
            sf::Vector2i mouseInMapWindow = sf::Mouse::getPosition(m_mapWindow);

            if (mouseInMapWindow.x > 0 && mouseInMapWindow.x < (int)m_mapWindow.getSize().x &&
				mouseInMapWindow.y > 0 && mouseInMapWindow.y < (int)m_mapWindow.getSize().y)
			{
				sf::Vector2f worldCoords = m_mapWindow.mapPixelToCoords(mouseInMapWindow);
				sf::Vector2i mouseTileCoords = {
					(int)(worldCoords.x / TILE_SIZE),
					(int)(worldCoords.y / TILE_SIZE)
				};

				m_map.setTile(mouseTileCoords.x, mouseTileCoords.y, '.');
			}

			m_editTimer.restart();
        }
        else if (sf::Mouse::isButtonPressed(sf::Mouse::Middle) && m_editTimer.getElapsedTime() > sf::milliseconds(100))
        {
            sf::Vector2i mouseInMapWindow = sf::Mouse::getPosition(m_mapWindow);

            if (mouseInMapWindow.x > 0 && mouseInMapWindow.x < (int)m_mapWindow.getSize().x &&
				mouseInMapWindow.y > 0 && mouseInMapWindow.y < (int)m_mapWindow.getSize().y)
			{
				sf::Vector2f worldCoords = m_mapWindow.mapPixelToCoords(mouseInMapWindow);
				sf::Vector2i mouseTileCoords = {
					(int)(worldCoords.x / TILE_SIZE),
					(int)(worldCoords.y / TILE_SIZE)
				};

				m_selectedCharIndex = m_map.getTileIndex(mouseTileCoords.x, mouseTileCoords.y);
			}

			m_editTimer.restart();
        }
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && m_editTimer.getElapsedTime() > sf::milliseconds(100))
			saveFile();
	}

	void saveFile() const
	{
		sf::Vector2i playerCoords {
			(int)(m_entities.at('p').getPosition().x / TILE_SIZE),
			((int)(m_entities.at('p').getPosition().y / TILE_SIZE)) + 1
		};

		std::ofstream stream(levelsPath + "level.txt");
		if (!stream)
        {
            std::cerr << "couldnt create save file stream!" << std::endl;
            return;
        }

		for (int y = 0; y < m_map.GRID_HEIGHT; ++y)
		{
			for (int x = 0; x < m_map.GRID_WIDTH; ++x)
			{
				if (x == playerCoords.x && y == playerCoords.y)
					stream << "p ";
				else
					stream << m_map.m_grid[x][y] << " ";
			}

			if (y + 1 < m_map.GRID_HEIGHT)
				stream << '\n';
		}

		sf::sleep(sf::milliseconds(100));
	}

	void close()
	{
	    m_tilesWindow.close();
	}

private:
	sf::RenderWindow& m_mapWindow;

	Map& m_map;

	std::map<char, const GameObject&> m_entities;

	const sf::Texture& m_texture;

	sf::RenderWindow m_tilesWindow;
	const std::size_t m_tilesPerRow = 3;
	const std::size_t m_tilesPerCol = 1;
	std::map<char, sf::Sprite> m_tilesSprites;

	char m_selectedCharIndex = 'a';
	sf::Clock m_editTimer;	// prevent from spamming
};
