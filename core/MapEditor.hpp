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
	MapEditor(GameScene& gs, TilesManager& tm);
	~MapEditor();

	void render();
	void handleInputs();
	void handleMapWindowEvent(const sf::Event& event);
	void handleTilesWindowEvents();
	void saveFile() const;
	void close();

private:
	GameScene& m_gs;

	sf::RenderWindow m_tilesWindow;
	const std::size_t m_tilesPerRow = 4;
	const std::size_t m_tilesPerCol = 1;
	std::vector<sf::Sprite> m_tilesSprites;

	const Tile* m_selectedTile;	// Can't be nullptr
	TilesManager& m_tilesMgr;
	sf::Clock m_editTimer;	// prevent from spamming
};
