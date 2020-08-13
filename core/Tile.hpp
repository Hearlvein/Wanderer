#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

class Tile
{
public:
	// In-class definition
	enum class Property
	{
		Void = 0,
		Solid,
		Ladder
	};

	Tile(const std::string& name, char index, const sf::Vector2f& texCoords, Property tp)
		: m_name(name), m_index(index), m_texCoords(texCoords), m_property(tp) {}

	// Getters
	const std::string& 	getName		() 	const { return m_name;		}
	char 				getIndex	() 	const { return m_index;		}
	const sf::Vector2f& getTexCoords() 	const { return m_texCoords;	}
	Property 			getProperty	() 	const { return m_property;	}

	// Helpers
	static char getTileIndexFromString(const std::string& str)
	{
	    return str.front();
	}

	static Property getPropertyFromString(const std::string& str)
	{
		if 		(str == "Void")		return Property::Void;
		else if (str == "Solid")	return Property::Solid;
		else if (str == "Ladder")	return Property::Ladder;
		else
		{
			std::cerr << "Couldn't get tile property from: \'" << str << "\'" << std::endl;
			return Property::Void;
		}
	}

private:
	// Identity
	std::string m_name;			// Optional, must for clarity's sake
	char m_index;				// Used to parse level map file

	sf::Vector2f m_texCoords;	// Position in tileset
	Property m_property;		// Tile property
};
