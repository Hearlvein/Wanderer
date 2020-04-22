#pragma once

#include <SFML/Graphics.hpp>
#include "GameObject.hpp"

class AnimatedEntity : public virtual GameObject
{
public:
	AnimatedEntity();
	virtual ~AnimatedEntity();

	virtual void update(float dt);
	void createSeq(const std::string& seqName, const sf::Vector2i& firstTile, std::size_t seqSize);
	void setCurrentSeq(const std::string& seqName);
	const sf::Vector2i& getTilePosition();

private:
	float m_elapsedTime = 0.f;
	std::string m_currentSeq;
	std::size_t m_currentTile;
	const float m_maxElapsedTime = 0.2f;
	std::map<std::string, std::vector<sf::Vector2i> > m_seqList;
};
