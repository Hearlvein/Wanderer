#include "Game.hpp"
#include "debug.hpp"

Game::Game()
{
	openLog();
	m_sceneManager.run();
	closeLog();
}

Game::~Game()
{

}
