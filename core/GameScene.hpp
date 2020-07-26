#pragma once

#include "Scene.hpp"
#include "Map.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Layer.hpp"
#include "Background.hpp"
#include "MapEditor.hpp"
#include <list>
#include <memory>

class GameScene : public Scene
{
public:
	GameScene(sf::RenderWindow* window);
	virtual ~GameScene();

	virtual void handleEvent(const sf::Event& event);
	virtual void checkInput();
	virtual void update(float dt);
	virtual void draw(sf::RenderTarget& target);

	void addEntity(const std::string& name, const sf::Vector2f& position);
	void moveEntity(MovingCharacter& entity, bool* xCollision = nullptr);
	void updateClimbingState(MovingCharacter& entity);
	void moveEnemy(Enemy& enemy);
	void moveCamera();

private:
	// resources
	sf::Texture m_tileset;
	sf::Texture m_backgroundTexture;

	Background m_background;

	const float m_screenPadding = 300.f;
	Map m_map;

	Player m_player;
	std::list<Enemy> m_enemiesList;

	std::map<std::string, Layer> m_layers;

	bool m_imguiEnabled = true;
	float m_lastDt = 0.f;

	mutable std::unique_ptr<MapEditor> m_mapEditor = nullptr;
};
