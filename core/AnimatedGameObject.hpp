#pragma once

#include <SFML/Graphics.hpp>
#include "GameObject.hpp"

struct TextureAndHitbox
{
	sf::IntRect textureRect;
	sf::FloatRect hitbox;
};

class AnimatedGameObject : public virtual GameObject
{
public:
	AnimatedGameObject();
	virtual ~AnimatedGameObject();

	virtual void update(float dt);
	void addTextureRectAndHitbox(const std::string& seqName, const sf::IntRect& textureRect, const sf::FloatRect& hitbox);
	void setCurrentSeq(const std::string& seqName);
	const sf::IntRect& getCurrentTextureRect();

private:
	float m_elapsedTime = 0.f;
	std::string m_currentSeq;
	std::size_t m_currentTile;
	const float m_maxElapsedTime = 0.2f;
	std::map<std::string, std::vector<TextureAndHitbox> > m_seqList;
};
