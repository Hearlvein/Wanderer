#include "MapEditor.hpp"
#include "Constants.hpp"
#include <imgui-SFML.h>
#include <imgui.h>
#include <iomanip>


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
	// Update map window read events state
	if (ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered())
		m_gs.setReadEvents(false);
	else
	{
		m_gs.setReadEvents(true);

		// Hover
		auto& w = *(m_gs.m_window);
		sf::Vector2i mouse = sf::Mouse::getPosition(w);
		// Mouse should be in the window to enable hovering
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
	}

	

	// ---------------- ImGUI stuff ----------------
	using namespace ImGui;

	Begin("Map Editor");

	Text("Save or load level");
	InputText("Level filename", m_levelFilenameBuffer, 64);
	if (Button("Save level"))
		saveLevel(levelsPath + m_levelFilenameBuffer);
	SameLine();
	if (Button("Load level"))
		m_gs.loadLevel(levelsPath + (const std::string)m_levelFilenameBuffer);


	Text("Tiles window");
	for (std::size_t i = 0; i < m_tilesSprites.size(); ++i)
	{
		PushID(i);
		if (ImageButton(m_tilesSprites[i]))
		{
			auto tileIt = m_tilesMgr.getTiles().begin();
			std::advance(tileIt, i);
			m_selectedTile = &(*tileIt);
			updateHover();
		}
		PopID();

		if (i+1 < m_tilesSprites.size())
			SameLine(0.f, 0.f);
	}

	
	Text("Center camera on player");
	Checkbox("Center on player", &(m_gs.m_cameraOnPlayer));

	End();	// Map editor
}

void MapEditor::handleInputs()
{
	// ------ map window inputs below ------

	if (ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered())
        return;

	// Pre-requisites: window active (with pointer inside) and edit delay respected
	if (!m_gs.m_window->hasFocus() ||
		m_editTimer.getElapsedTime() < m_editDelay)
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
	// Middle mouse button selection
	if (event.type == sf::Event::MouseButtonPressed &&
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

void MapEditor::saveLevel(const std::string& levelFilename) const
{
	// Saving map
	std::ofstream mapSaveStream(levelFilename + "/map.txt");
	if (mapSaveStream)
	{
		for (int y = 0; y < m_gs.m_map.GRID_HEIGHT; ++y)
		{
			for (int x = 0; x < m_gs.m_map.GRID_WIDTH; ++x)
				mapSaveStream << m_gs.m_map.getTileIndex(x, y) << " ";

			if (y + 1 < m_gs.m_map.GRID_HEIGHT)
				mapSaveStream << '\n';
		}
	}
	else
		std::cerr << "couldn't create map save file stream!" << std::endl;


	// Saving entities
	std::ofstream entitiesSaveStream(levelFilename + "/entities.json");
	if (entitiesSaveStream)
	{
		nlohmann::json data;

		const auto& player = *(m_gs.m_player);
		data["player"] = { player.getPosition().x, player.getPosition().y };

		data["enemies"] = {};
		for (const Enemy* enemy : m_gs.m_enemies)
		{
			std::vector<float> vec_pos = { enemy->getPosition().x, enemy->getPosition().y };
			data["enemies"].emplace_back(std::move(vec_pos));
		}

		entitiesSaveStream << std::setw(4) << data;
	}
	else
		std::cerr << "couldn't create entities save file stream!" << std::endl;
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