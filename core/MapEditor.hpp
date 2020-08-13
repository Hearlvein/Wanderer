#pragma once

#include "Map.hpp"
#include "GameObject.hpp"
#include "Constants.hpp"
#include "GameScene.hpp"
#include "Tile.hpp"

#include <SFML/Graphics.hpp>
#include <fstream>
#include <cmath>
#include <map>

class Map;

class MapEditor
{
public:
	MapEditor(GameScene& gs, TilesManager& tm)
		: m_gs(gs)
		, m_tilesMgr(tm)
	{
		m_tilesWindow.create({ m_tilesPerRow * (unsigned)TILE_SIZE, m_tilesPerCol * (unsigned)TILE_SIZE }, "Tiles", sf::Style::Titlebar);
		m_tilesWindow.setPosition({ 0, 0, });

		m_tilesSprites['a'] = sf::Sprite(m_gs.m_tileset, { (int)TILE_SIZE, 0, (int)TILE_SIZE, (int)TILE_SIZE });
		m_tilesSprites['a'].setPosition({ 0.f, 0.f });

		m_tilesSprites['b'] = sf::Sprite(m_gs.m_tileset, { 2 * (int)TILE_SIZE, 0, (int)TILE_SIZE, (int)TILE_SIZE });
		m_tilesSprites['b'].setPosition({ TILE_SIZE, 0.f });

		m_tilesSprites['l'] = sf::Sprite(m_gs.m_tileset, { 3 * (int)TILE_SIZE, 0, (int)TILE_SIZE, (int)TILE_SIZE });
		m_tilesSprites['l'].setPosition({ 2 * TILE_SIZE, 0.f });

		
		m_selectedTile = m_tilesMgr.getDefaultTile();
	}
	~MapEditor() {}

	void render()
	{
		m_tilesWindow.clear();

		for (auto it = m_tilesSprites.begin(); it != m_tilesSprites.end(); ++it)
			m_tilesWindow.draw(it->second);

		m_tilesWindow.display();
	}

	void handleInputs()
	{
		// ------ map window inputs below ------

		// Pre-requisites: window active (with pointer inside) and edit delay respected
		if (!m_gs.m_window->hasFocus() ||
			m_editTimer.getElapsedTime() < sf::milliseconds(50))
			return;

		// mouse management
		char mouseCode = -1;	// unhandled value
		if		(sf::Mouse::isButtonPressed(sf::Mouse::Left))  mouseCode = 0;
		else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) mouseCode = 1;

		if (mouseCode != -1)	// defined behavior: need to handle mouse
		{
			sf::Vector2f worldCoords = m_gs.m_window->mapPixelToCoords(
				sf::Mouse::getPosition(*m_gs.m_window), m_gs.m_window->getView());

			sf::Vector2i mouseTileCoords{
				(int)std::floor(worldCoords.x / TILE_SIZE),
				(int)std::floor(worldCoords.y / TILE_SIZE)
			};

			// To compare previous and after states
			sf::Vector2i previousGridSize{
				m_gs.m_map.GRID_WIDTH,
				m_gs.m_map.GRID_HEIGHT,
			};

			// Hint to know how to translate entities and view
			bool newColLeft = false;
			bool newRowTop = false;

			const Tile* newTile = (mouseCode == 0)
				? m_tilesMgr.getDefaultTile()
				: m_selectedTile;
			m_gs.m_map.setTile(mouseTileCoords.x, mouseTileCoords.y, newTile, &newColLeft, &newRowTop);

			// Translate entities and view if needed
			sf::Vector2f translation;
			if (newColLeft)	translation.x = (m_gs.m_map.GRID_WIDTH  - previousGridSize.x) * TILE_SIZE;
			if (newRowTop)	translation.y = (m_gs.m_map.GRID_HEIGHT - previousGridSize.y) * TILE_SIZE;

			if (translation != sf::Vector2f())
			{
				std::cout << "translating" << std::endl;

				for (GameObject* entity : m_gs.m_entities)
					entity->move(translation);

				sf::View v = m_gs.m_window->getView();
				v.move(-translation);
				m_gs.m_window->setView(v);
			}

			m_editTimer.restart();
		}
	}

	void handleMapWindowEvent(const sf::Event& event)
	{
		// keyboard events
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::S)		// S key: save map to file
				saveFile();

			else if (event.key.code == sf::Keyboard::R)	// R key: reload level from file
				m_gs.load();
		}

		// mouse middle button event (copy tile)
		else if (event.type == sf::Event::MouseButtonPressed &&
			event.mouseButton.button == sf::Mouse::Middle)
		{
			sf::Vector2f worldCoords = m_gs.m_window->mapPixelToCoords(
				sf::Mouse::getPosition(*m_gs.m_window), m_gs.m_window->getView());

			sf::Vector2i mouseTileCoords{
				(int)std::floor(worldCoords.x / TILE_SIZE),
				(int)std::floor(worldCoords.y / TILE_SIZE)
			};

			m_selectedTile = m_gs.m_map.getTile(mouseTileCoords.x, mouseTileCoords.y);
		}
	}

	void handleTilesWindowEvents()
	{
		sf::Event e;
		while (m_tilesWindow.pollEvent(e))
		{
			// TODO: handle m_tilesWindow.close()?? (invalidates GameScene.m_mapEditor pointer?)

			if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
			{
				int index = static_cast<int>(e.mouseButton.x / TILE_SIZE);
				if      (index == 0) m_selectedTile = m_tilesMgr.getTileFromIndex('a');
				else if (index == 1) m_selectedTile = m_tilesMgr.getTileFromIndex('b');
				else if (index == 2) m_selectedTile = m_tilesMgr.getTileFromIndex('l');
			}
		}
	}

	void saveFile() const
	{
		auto getEntityCoords = [](const GameObject* go)
		{
			return sf::Vector2i(
				(int)(go->getPosition().x / TILE_SIZE),
				((int)(go->getPosition().y / TILE_SIZE)) + 1
			);
		};

		std::multimap<char, sf::Vector2i> entitiesCoords;
		entitiesCoords.emplace('p', getEntityCoords(m_gs.m_player));
		for (auto e : m_gs.m_enemies)
			entitiesCoords.emplace('e', getEntityCoords(e));

        auto getCharIndex = [&](int x, int y) -> char
        {
            for (auto it = entitiesCoords.begin(); it != entitiesCoords.end(); ++it)
            {
                if (it->second == sf::Vector2i(x, y))
                    return it->first;
            }

            return m_gs.m_map.getTileIndex(x, y);
        };

		std::ofstream stream(levelsPath + "level.txt");
		if (!stream)
		{
			std::cerr << "couldn't create save file stream!" << std::endl;
			return;
		}

		for (int y = 0; y < m_gs.m_map.GRID_HEIGHT; ++y)
		{
			for (int x = 0; x < m_gs.m_map.GRID_WIDTH; ++x)
				stream << getCharIndex(x, y) << " ";

			if (y + 1 < m_gs.m_map.GRID_HEIGHT)
				stream << '\n';
		}
	}

	void close()
	{
		m_tilesWindow.close();
	}

private:
	GameScene& m_gs;

	sf::RenderWindow m_tilesWindow;
	const std::size_t m_tilesPerRow = 3;
	const std::size_t m_tilesPerCol = 1;
	std::map<char, sf::Sprite> m_tilesSprites;

	const Tile* m_selectedTile;	// Can't be nullptr
	TilesManager& m_tilesMgr;
	sf::Clock m_editTimer;	// prevent from spamming
};
