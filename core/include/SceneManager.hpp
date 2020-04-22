#pragma once

#include <SFML/Graphics.hpp>
#include "Scene.hpp"

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	void setCurrentScene(Scene* currentScene);
	void run();

private:
	sf::RenderWindow m_window;
	Scene* m_currentScene = nullptr;
};

