#include "Scene.hpp"

#include <iostream>

Scene::Scene(sf::RenderWindow* window)
	: m_window(window)
{
	std::cerr << "Creating a new scene (this=" << this << ")" << std::endl;
}

Scene::~Scene()
{
	std::cerr << "Destroying a scene (this=" << this << ")" << std::endl;
}
