#pragma once

#include "Scene.hpp"
#include "Map.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Fireball.hpp"
#include "Layer.hpp"
#include "BackgroundManager.hpp"
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

	void moveEntity(MovingGameObject& entity, bool* xCollision = nullptr);
	void updateClimbingState(MovingGameObject& entity);
	void moveEnemy(Enemy& enemy);
	// bool moveFireball(std::list<Fireball>::iterator& fb);
	void moveCamera();
	// void addFireball();

private:
	friend class GameObject;

	BackgroundManager m_bgMgr;

	const float m_screenPadding = 300.f;
	sf::Texture m_texture;
	Map m_map;

	Player m_player;
	std::list<Enemy> m_enemiesList;
	std::list<Fireball> m_fireballs;

	std::map<std::string, Layer> m_layers;

	bool m_imguiEnabled = true;
	float m_dt;
};
