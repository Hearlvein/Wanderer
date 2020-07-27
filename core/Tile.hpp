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
	sf::Vector2f texCoords;
	TileProperty property = Ladder;

	static const Tile s_Void;
	static const Tile s_Block1;
	static const Tile s_Block2;
	static const Tile s_Ladder;

	static const Tile& getTileFromIndex(char i)
	{
		switch (i)
		{
			case '.': 	return Tile::s_Void;
			case 'a': 	return Tile::s_Block1;
			case 'b':	return Tile::s_Block2;
			case 'l': 	return Tile::s_Ladder;
			default:	return Tile::s_Ladder;
		}
	}

	// Debugging purpose (printing map in console)
	// TODO: remove
	static char getPropertyCharIndexFromTile(const Tile& t)
	{
		switch (t.property)
		{
		case TileProperty::Void:	return '.';
		case TileProperty::Solid:	return '#';
		case TileProperty::Ladder:	return '|';
		default:					return '?';
		}
	}
};
