#include "AnimatedGameObject.hpp"
#include <iostream>

AnimatedGameObject::AnimatedGameObject()
{
	// std::cout << "animated entity constructor" << std::endl;
}

AnimatedGameObject::~AnimatedGameObject()
{

}

void AnimatedGameObject::update(float dt)
{
	m_elapsedTime += dt;
	if (m_elapsedTime >= m_maxElapsedTime)
	{
		if (++m_currentTile >= m_seqList[m_currentSeq].size())
			m_currentTile = 0;

		setTextureRect(getCurrentTextureRect());
		m_elapsedTime = 0.f;

		// std::cout << "animated entity needs update" << std::endl;
	}
}

void AnimatedGameObject::addTextureRectAndHitbox(const std::string& seqName, const sf::IntRect& textureRect, const sf::FloatRect& hitbox)
{
	m_seqList[seqName].push_back({textureRect, hitbox});
}

void AnimatedGameObject::setCurrentSeq(const std::string& seqName)
{
	if (m_seqList.find(seqName) == m_seqList.end())
		std::cout << "error: \'" << seqName << "\' doesn't exist in m_seqList" << std::endl;

	if (seqName == m_currentSeq)
		return;

	m_currentSeq = seqName;
	m_currentTile = 0;
	setTextureRect(getCurrentTextureRect());
}

const sf::IntRect& AnimatedGameObject::getCurrentTextureRect()
{
	return m_seqList[m_currentSeq][m_currentTile].textureRect;
}
