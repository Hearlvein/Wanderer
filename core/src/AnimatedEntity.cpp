#include "AnimatedEntity.hpp"
#include <iostream>

AnimatedEntity::AnimatedEntity()
{
	// std::cout << "animated entity constructor" << std::endl;
}

AnimatedEntity::~AnimatedEntity()
{

}

void AnimatedEntity::update(float dt)
{
	m_elapsedTime += dt;
	if (m_elapsedTime >= m_maxElapsedTime)
	{
		if (++m_currentTile >= m_seqList[m_currentSeq].size())
			m_currentTile = 0;

		setTexturePosition(getTilePosition());
		m_elapsedTime = 0.f;

		// std::cout << "animated entity needs update" << std::endl;
	}
}

void AnimatedEntity::createSeq(const std::string& seqName, const sf::Vector2i& firstTile, std::size_t seqSize)
{
	m_seqList[seqName].resize(seqSize);
	for (std::size_t i = 0; i < seqSize; ++i)
		m_seqList[seqName][i] = sf::Vector2i(static_cast<float>(i) * getSize().x + firstTile.x, firstTile.y);
}

void AnimatedEntity::setCurrentSeq(const std::string& seqName)
{
	if (m_seqList.find(seqName) == m_seqList.end())
		std::cout << "error: \'" << seqName << "\' doesn't exist in m_seqList" << std::endl;

	if (seqName == m_currentSeq)
		return;

	m_currentSeq = seqName;
	m_currentTile = 0;
	setTexturePosition(getTilePosition());
}

const sf::Vector2i& AnimatedEntity::getTilePosition()
{
	return m_seqList[m_currentSeq][m_currentTile];
}
