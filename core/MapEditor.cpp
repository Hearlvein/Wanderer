#include "MapEditor.hpp"

MapEditor::MapEditor(GameScene& gs, TilesManager& tm)
	: m_gs(gs)
	, m_tilesMgr(tm)
{
	// Deactivate auto camera
	m_gs.setCameraOnPlayer(false);

	m_tilesWindow.create({ m_tilesPerRow * (unsigned)TILE_SIZE, m_tilesPerCol * (unsigned)TILE_SIZE }, "Tiles", sf::Style::Titlebar);
	m_tilesWindow.setPosition({ 0, 0, });

	const auto& tiles = m_tilesMgr.getTiles();
	int iterator_distance = 0;
	for (auto it = tiles.begin(); it != tiles.end(); ++it)
	{
		m_tilesSprites.emplace_back(
			m_gs.m_tileset,
			sf::IntRect(it->getTexCoords().x, it->getTexCoords().y, TILE_SIZE, TILE_SIZE)
		);
		m_tilesSprites.back().setPosition(iterator_distance * TILE_SIZE, 0.f);

		++iterator_distance;
	}


	m_selectedTile = m_tilesMgr.getTileFromIndex('a');
}
MapEditor::~MapEditor()
{
	// Re-activate auto camera
	m_gs.setCameraOnPlayer();
}

void MapEditor::render()
{
	m_tilesWindow.clear();

	for (auto it = m_tilesSprites.begin(); it != m_tilesSprites.end(); ++it)
		m_tilesWindow.draw(*it);

	m_tilesWindow.display();
}

void MapEditor::handleInputs()
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

			m_gs.moveCamera(translation);
		}

		m_editTimer.restart();
	}
}

void MapEditor::handleMapWindowEvent(const sf::Event& event)
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

void MapEditor::handleTilesWindowEvents()
{
	sf::Event e;
	while (m_tilesWindow.pollEvent(e))
	{
		// TODO: handle m_tilesWindow.close()?? (invalidates GameScene.m_mapEditor pointer?)

		if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
		{
			// count = y * width + x
			int x = e.mouseButton.x / TILE_SIZE;
			int y = e.mouseButton.y / TILE_SIZE;
			//std::cout << "click at: " << x << " ; " << y << std::endl;

			int iterator_distance = static_cast<int>(y * m_tilesPerRow + x);
            //std::cout << "iterator dist=" << iterator_distance << std::endl;
			const auto& tiles = m_tilesMgr.getTiles();
			auto it = tiles.cbegin();
			std::advance(it, iterator_distance);
			m_selectedTile = &(*it);
		}
	}
}

void MapEditor::saveFile() const
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

void MapEditor::close()
{
	m_tilesWindow.close();
}
