#include "Game.hpp"
#include "Utility/debug.hpp"

Game::Game()
{
	openLog();
	m_sceneManager.run();
	closeLog();
}

Game::~Game()
{

}
