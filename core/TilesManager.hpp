#pragma once

#include "Tile.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <list>

class TilesManager
{
public:
	TilesManager()
	{

	}
	~TilesManager()
	{

	}

	void loadTiles(const std::string& filename)
	{
		std::ifstream stream(filename);
		if (stream)
		{
			nlohmann::json data;
			stream >> data;

			// Browse tiles
			for (auto& tileData : data["tiles"])
			{
				m_tiles.emplace_back(
					(std::string)tileData["name"],
					Tile::getTileIndexFromString(tileData["index"]),
					sf::Vector2f(tileData["texCoords"][0], tileData["texCoords"][1]),
					Tile::getPropertyFromString(tileData["property"])
				);
			}

			m_defaultTile = getTileFromIndex('.');
		}
	}

	void clearTiles()
	{
		m_tiles.clear();
	}

	const Tile* getTileFromIndex(char index) const
	{
		for (auto& tile : m_tiles)
		{
			if (tile.getIndex() == index)
				return &tile;
		}

		std::cerr << "Critical error: can't get tile from index(" << index << ")\n" << std::flush;
		assert(false);
		return nullptr;
	}

	const Tile* getDefaultTile() const 	{ return m_defaultTile; }

	const std::list<Tile>& getTiles() const { return m_tiles; }

private:
	std::list<Tile> m_tiles;
	const Tile* m_defaultTile = nullptr;	// Usually VoidTile (see ctor). TODO: need to handle that dynamically at runtime
};
