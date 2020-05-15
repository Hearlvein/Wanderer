#pragma once

#include <SFML/Graphics.hpp>
#include "SceneManager.hpp"

class Game
{
public:
	Game();
	~Game();

private:
	SceneManager m_sceneManager;
};

