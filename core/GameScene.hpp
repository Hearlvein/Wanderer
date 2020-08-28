#pragma once

#include "Scene.hpp"
#include "Map.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Layer.hpp"
#include "Background.hpp"
#include "TilesManager.hpp"

#include <list>
#include <memory>

class MapEditor;

/** The GameScene is composed of a map and entities, include the player **/
class GameScene : public Scene
{
public:
	GameScene(sf::RenderWindow* window);
	virtual ~GameScene();

	void load(const std::string& levelFilename);
	void destroyEntities();
	void setReadEvents(bool read);
	bool getReadEvents() const;

	virtual void handleEvent(const sf::Event& event) override;
	virtual void checkInput() override;
	virtual void update(float dt) override;
	virtual void draw(sf::RenderTarget& target) override;

	// ----- Entity management -----
	void addEntity(const std::string& name, const sf::Vector2f& position);
	void moveEntity(MovingCharacter& entity, bool* xCollision = nullptr);
	void updateClimbingState(MovingCharacter& entity);
	void moveEnemy(Enemy& enemy);

	// ----- Camera management -----
	void setCameraOnPlayer(bool v = true);
	void updateCamera();
	void placeCameraOnPlayer();
	void moveCamera(const sf::Vector2f& translation);
	void moveCameraIfMouseOnEdge();

private:
	friend class MapEditor;

	// resources
	sf::Texture m_tileset;
	sf::Texture m_backgroundTexture;

	// Layers
	std::map<std::string, Layer> m_layers;
	Background m_background;
	Map m_map;

	// entities storage
	std::list<GameObject*> m_entities;	// ownership of heap pointers
	Player* m_player = nullptr;
	std::list<Enemy*> m_enemies;

	// Behavior
	bool m_readEvents = true;
	bool m_imguiEnabled = true;
	float m_lastDt = 0.f;
	bool m_cameraOnPlayer = true;
	const float m_screenPadding = 300.f;
	const unsigned int m_mouseScreenPadding = 50;

	TilesManager m_tilesMgr;
	mutable MapEditor* m_mapEditor = nullptr;
};
