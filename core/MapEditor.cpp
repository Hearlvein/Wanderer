#include "MapEditor.hpp"
#include <imgui-SFML.h>
#include <imgui.h>

MapEditor::MapEditor(GameScene& gs, TilesManager& tm)
	: m_gs(gs)
	, m_tilesMgr(tm)
{
	// Deactivate auto camera
	m_gs.setCameraOnPlayer(false);

	const auto& tiles = m_tilesMgr.getTiles();
	m_tilesPerRow = tiles.size();
	int iterator_distance = 0;
	for (auto it = tiles.begin(); it != tiles.end(); ++it)
	{
		m_tilesSprites.emplace_back(
			m_gs.m_tileset,
			sf::IntRect(
				(int)it->getTexCoords().x,
				(int)it->getTexCoords().y,
				(int)TILE_SIZE,
				(int)TILE_SIZE
			)
		)
		.setPosition(iterator_distance * TILE_SIZE, 0.f);

		++iterator_distance;
	}

	// Makes main window active. By default last created only is active
	m_gs.m_window->requestFocus();

	m_selectedTile = m_tilesMgr.getTileFromIndex('a');
	updateHover();
}
MapEditor::~MapEditor()
{
	// Re-activate auto camera
	m_gs.setCameraOnPlayer();
}

void MapEditor::render()
{
	// Hover
	auto& w = *(m_gs.m_window);
	sf::Vector2i mouse = sf::Mouse::getPosition(w);
	if (mouse.x >= 0 &&
		mouse.y >= 0 &&
		(unsigned)mouse.x < w.getSize().x &&
		(unsigned)mouse.y < w.getSize().y)
	{
		sf::Vector2f coords = w.mapPixelToCoords(mouse);
		sf::Vector2f floored(
			std::floor(coords.x / TILE_SIZE) * TILE_SIZE,
			std::floor(coords.y / TILE_SIZE) * TILE_SIZE
		);
		m_hover.setPosition(floored);
		w.draw(m_hover);
	}

	ImGui::Begin("Tiles window");
	for (std::size_t i = 0; i < m_tilesSprites.size(); ++i)
    {
        ImGui::PushID(753 + i);
        if (ImGui::ImageButton(m_tilesSprites[i]))
        {
            auto tileIt = m_tilesMgr.getTiles().begin();
            std::advance(tileIt, i);
            m_selectedTile = &(*tileIt);
            updateHover();
            std::cout << "clicked; dist=" << i << std::endl;
        }
        ImGui::PopID();
        ImGui::SameLine(0.f, 0.f);
    }
    ImGui::End();
}

void MapEditor::handleInputs()
{
	// ------ map window inputs below ------

	if (ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered())
        return;

	// Pre-requisites: window active (with pointer inside) and edit delay respected
	if (!m_gs.m_window->hasFocus() ||
		m_editTimer.getElapsedTime() < sf::milliseconds(50))
		return;

	// mouse management
	char mouseCode = -1;	// unhandled value
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		mouseCode = 0;
	else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
		mouseCode = 1;
	else
		return;

	// mouseCode != -1	// defined behavior: need to handle mouse
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
		updateHover();
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

void MapEditor::updateHover()
{
	if (!m_selectedTile)
		return;

	m_hover = sf::Sprite(
		m_gs.m_tileset,
			sf::IntRect(
				(int)m_selectedTile->getTexCoords().x,
				(int)m_selectedTile->getTexCoords().y,
				(int)TILE_SIZE,
				(int)TILE_SIZE
	));
	m_hover.setColor(sf::Color(255, 255, 255, 128));
}