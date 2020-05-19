#pragma once

#include <SFML/Graphics.hpp>

enum TileProperty
{
	Void = 0,
	Solid,
	Ladder
};

struct Tile
{
	sf::Vector2f texturePosition;
	TileProperty property = Ladder;

	static const Tile s_Void;
	static const Tile s_Block1;
	static const Tile s_Block2;
	static const Tile s_Ladder;

	static Tile getTileFromIndex(char i)
	{
		switch (i)
		{
			case '.': 	return Tile::s_Void;
			case 'a': 	return Tile::s_Block1;
			case 'b':		return Tile::s_Block2;
			case 'l': 	return Tile::s_Ladder;
			default:	return Tile::s_Ladder;
		}
	}
};
