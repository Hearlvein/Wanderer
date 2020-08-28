#pragma once

#include "Scene/Map.hpp"
#include "Entity/GameObject.hpp"
#include "Constants.hpp"
#include "Scene/GameScene.hpp"
#include "Scene/Tile.hpp"

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
	void saveFile(const std::string& levelFilename) const;
	void updateHover();

private:
	GameScene& m_gs;

	// TODO: don't use these values yet
	std::size_t m_tilesPerRow = 4;
	std::size_t m_tilesPerCol = 1;
	std::vector<sf::Sprite> m_tilesSprites;

	char m_levelFilenameBuffer[64] = "";
	const Tile* m_selectedTile;	// Can't be nullptr
	sf::Sprite m_hover;
	TilesManager& m_tilesMgr;
	sf::Time m_editDelay = sf::milliseconds(10);
	sf::Clock m_editTimer;	// prevent from spamming
};
