#pragma once

#include "Scene.hpp"
#include "Map.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Layer.hpp"
#include "Background.hpp"

#include <list>
#include <memory>

class MapEditor;

/** The GameScene is composed of a map and entities, include the player **/
class GameScene : public Scene
{
public:
	GameScene(sf::RenderWindow* window);
	virtual ~GameScene();

	void load();
	void destroyEntities();

	virtual void handleEvent(const sf::Event& event) override;
	virtual void checkInput() override;
	virtual void update(float dt) override;
	virtual void draw(sf::RenderTarget& target) override;

	void addEntity(const std::string& name, const sf::Vector2f& position);
	void moveEntity(MovingCharacter& entity, bool* xCollision = nullptr);
	void updateClimbingState(MovingCharacter& entity);
	void moveEnemy(Enemy& enemy);
	void moveCamera();

private:
	friend class MapEditor;

	// resources
	sf::Texture m_tileset;
	sf::Texture m_backgroundTexture;

	Background m_background;

	const float m_screenPadding = 300.f;
	Map m_map;

	// entities storage
	std::list<GameObject*> m_entities;	// ownership of heap pointers
	Player* m_player = nullptr;
	std::list<Enemy*> m_enemies;

	std::map<std::string, Layer> m_layers;

	bool m_imguiEnabled = true;
	float m_lastDt = 0.f;

	mutable MapEditor* m_mapEditor = nullptr;
};
