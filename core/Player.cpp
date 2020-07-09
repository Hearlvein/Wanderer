#include "Player.hpp"
#include "Constants.hpp"

Player::Player()
{
	AnimatedGameObject::loadFromFile(animationsPath + "player.txt");
	AnimatedGameObject::setCurrentAnimationName("right");
}

Player::~Player()
{

}
