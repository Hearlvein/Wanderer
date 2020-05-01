#include <iostream>
#include <imgui.h>
#include <imgui-SFML.h>
#include "SceneManager.hpp"
#include "GameScene.hpp"
#include "Constants.hpp"

SceneManager::SceneManager()
{
	m_window.create({1800, 1000}, "Wanderer");
	// m_window.create(sf::VideoMode((unsigned int)SCREEN_WIDTH, (unsigned int)SCREEN_HEIGHT), "Wanderer");
	//m_window.setFramerateLimit(60);
	m_window.setVerticalSyncEnabled(true);

	m_currentScene = new GameScene(&m_window);
}

SceneManager::~SceneManager()
{
	if (m_currentScene)
		delete m_currentScene;
}

void SceneManager::setCurrentScene(Scene* currentScene)
{
	if (m_currentScene)
		delete m_currentScene;

	m_currentScene = currentScene;
}

void SceneManager::run()
{
	ImGui::SFML::Init(m_window);

	sf::Clock clock;

	while (m_window.isOpen())
	{
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed)
				m_window.close();
			else
				m_currentScene->handleEvent(event);
		}

		m_currentScene->checkInput();

		sf::Time dt = clock.restart();
		ImGui::SFML::Update(m_window, dt);
		m_currentScene->update(dt.asSeconds());

		m_window.clear();

		m_currentScene->draw(m_window);
		ImGui::SFML::Render(m_window);

		m_window.display();
	}

	ImGui::SFML::Shutdown();
}
