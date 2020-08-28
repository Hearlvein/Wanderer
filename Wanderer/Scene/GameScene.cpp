#include "MapEditor.hpp"
#include "Scene/GameScene.hpp"
#include "Constants.hpp"
#include "Utility/util.hpp"
#include "Utility/debug.hpp"

#include <functional>
#include <iostream>
#include <cassert>
#include <cmath>

#include <imgui-SFML.h>
#include <imgui.h>

GameScene::GameScene(sf::RenderWindow* window)
	: Scene(window)
	, m_map(m_tilesMgr)
{
	// ------------------- textures (resource: loaded once) -------------------
	m_tileset.loadFromFile(texturesPath + "tileset.png");
	m_backgroundTexture.loadFromFile(texturesPath + "background.png");

	// TODO: level.txt is some kind of default level to load. Should be customizable (levelData.json)
	load("level.txt");	// load map and entities

	// TODO: debug purpose
	m_mapEditor = new MapEditor(*this, m_tilesMgr);
}

GameScene::~GameScene()
{
	if (m_mapEditor)
		delete m_mapEditor;

	destroyEntities();
}

void GameScene::load(const std::string& levelFilename)
{
	// reset
	m_layers["backgroundLayer"].clear();
	m_layers["mapLayer"].clear();
	m_layers["mobsLayer"].clear();
	m_layers["playerLayer"].clear();
	destroyEntities();
	m_tilesMgr.clearTiles();
	// TODO: dirty: can't reset position because background changed as the player is (initially) centered (placeCameraOnPlayer)
	// m_background.resetPosition();

	// intialize

	// background
	m_background.getSprite()->setTexture(m_backgroundTexture);
	m_layers["backgroundLayer"].addObject(m_background.getDrawable());

	// map & entities
	// Important: load tiles before map because map uses the tiles (obviously!)
	m_tilesMgr.loadTiles(texturesPath + "tilesData.json");	// Setting up pointers before creating map

	m_map.setTexture(m_tileset);
	m_map.load(levelsPath + levelFilename, [&](const std::string& n, const sf::Vector2f& p)
	{
		addEntity(n, p);
	});
	m_layers["mapLayer"].addObject(&m_map);

	// reset camera
	m_window->setView(m_window->getDefaultView());

	// Player should be registered after loading
	assert(m_player);
}

void GameScene::destroyEntities()
{
	m_player = nullptr;
	m_enemies.clear();

	while (!m_entities.empty())
	{
		delete m_entities.back();
		m_entities.pop_back();
	}
}

void GameScene::setReadEvents(bool read)
{
	m_readEvents = read;
}

bool GameScene::getReadEvents() const
{
	return m_readEvents;
}

void GameScene::handleEvent(const sf::Event& event)
{
	if (!m_readEvents)
		return;

	if (event.type == sf::Event::Closed)
	{
		m_window->close();
		return;
	}
	else if (event.type == sf::Event::Resized)
	{
		sf::FloatRect visibleArea(0.f, 0.f, (float)event.size.width, (float)event.size.height);
		m_window->setView(sf::View(visibleArea));
	}

	// Mouse coordinates in world coordinates
	else if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::W)
		{
			auto mousePosition = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));
			std::cout << "mouse position: " << mousePosition.x << " ; " << mousePosition.y << std::endl;
		}
		else if (event.key.code == sf::Keyboard::E)
		{
			if (m_mapEditor)
			{
				delete m_mapEditor;
				m_mapEditor = nullptr;
			}
			else
				m_mapEditor = new MapEditor(*this, m_tilesMgr);
		}
	}


	if (m_mapEditor)
	{
		m_mapEditor->handleMapWindowEvent(event);
	}
}

void GameScene::checkInput()
{
	if (m_mapEditor)
	{
		m_mapEditor->handleInputs();
	}

	if (!m_readEvents)
		return;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		m_player->setFacing(Direction::Right);
		m_player->setWalkingState(WalkingState::Beginning);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		m_player->setFacing(Direction::Left);
		m_player->setWalkingState(WalkingState::Beginning);
	}
	else
		m_player->setWalkingState(WalkingState::End);


	// Check for jump
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		m_player->setYState(YState::Jumping);

	// Check for ladder
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && m_map.touchingTile(m_player->getHitbox(), Tile::Property::Ladder))
	{
		m_player->setYState(YState::Climbing);
		m_player->setClimbingDirection(Direction::Up);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && m_map.touchingTile(m_player->getHitbox(), Tile::Property::Ladder))
	{
		m_player->setYState(YState::Climbing);
		m_player->setClimbingDirection(Direction::Down);
	}
	else if (m_map.touchingTile(m_player->getHitbox(), Tile::Property::Ladder))
		m_player->setClimbingDirection(Direction::None);
}

void GameScene::update(float dt)
{
	m_lastDt = dt;

	// Enemy collision
	if (!m_player->isInvicible())
	{
		for (auto enemy : m_enemies)
		{
			if (boxesOverlapping(enemy->getHitbox(), m_player->getHitbox()))
			{
				m_player->takeDamage(20);
				m_player->setIsInvicible(true, 2.f);
			}
		}
	}

	// internal player update
	m_player->update(dt);

	// external player update
	updateClimbingState(*m_player);
	moveEntity(*m_player);

	// enemies update
	for (auto enemy : m_enemies)
	{
		enemy->update(dt);
		// no updateClimbingState because entities can't climb ladders
		moveEnemy(*enemy);
	}

	updateCamera();
}

void GameScene::updateClimbingState(MovingCharacter& entity)
{
	if (entity.getYState() == YState::Climbing && !m_map.touchingTile(m_player->getHitbox(), Tile::Property::Ladder))
		entity.setYState(YState::Falling);
}

void GameScene::addEntity(const std::string& name, const sf::Vector2f& position)
{
	if (name == "player")
	{
		// Allocation
		assert(!m_player);	// m_player should be initialized once per map loading
		m_player = new Player();
		m_entities.push_front(m_player);

		m_player->setPosition(position.x, position.y);
		m_player->setTexture(m_tileset);
		m_layers["playerLayer"].addObject(m_player);
	}
	else if (name == "enemy")
	{
		// Allocation
		Enemy* enemy = new Enemy();
		m_enemies.push_back(enemy);
		m_entities.push_back(enemy);

		enemy->setTexture(m_tileset);
		enemy->setPosition(position.x, position.y);
		m_layers["mobsLayer"].addObject(enemy);
	}
	else
		std::cerr << "!!! Calling placeEntity with name=" << name << "!!!" << std::endl;
}

void GameScene::moveEntity(MovingCharacter& entity, bool* xCollision)
{
	Box hitbox = entity.getHitbox();

	float dx = entity.getMovement().x;
	float dy = entity.getMovement().y;

	// Y checking
	if (dy != 0.f)
	{
		if (std::abs(dy) >= TILE_SIZE)
		{
			//std::cout << "dy=" << dy;
			dy = (TILE_SIZE - 1.f) * ((dy < 0) ? -1 : 1);	// maximum dy with care of sign
			//std::cout << " -> " << dy << std::endl;
		}

		if (!m_map.touchingTile({hitbox.x, hitbox.y+dy, hitbox.w, hitbox.h}, Tile::Property::Solid))
			hitbox.y += dy;
		else
		{
			if (entity.getYState() == YState::Falling)
			{
				hitbox.y = std::ceil((hitbox.y + hitbox.h - 1.f) / TILE_SIZE) * TILE_SIZE - hitbox.h;
				entity.setYState(YState::Grounded);
			}
			else if (entity.getYState() == YState::Jumping)
			{
				//std::cout << "block ahead keeping from jumping higher" << std::endl;
				entity.setYState(YState::Falling);
			}
		}
	}

	// X checking
	if (dx != 0.f)
	{
		if (std::abs(dx) >= TILE_SIZE)
		{
			//std::cout << "dx=" << dx;
			dx = (TILE_SIZE - 1.f) * ((dx < 0) ? -1 : 1);	// maximum dx with care of sign
			//std::cout << " -> " << dx << std::endl;
		}

		if (!m_map.touchingTile({hitbox.x+dx, hitbox.y, hitbox.w, hitbox.h}, Tile::Property::Solid))
			hitbox.x += dx;
		else
		{
			if (dx > 0)
				hitbox.x = std::ceil((hitbox.x + hitbox.w - 1.f) / TILE_SIZE) * TILE_SIZE - hitbox.w;
			else
				hitbox.x = std::floor(hitbox.x / TILE_SIZE) * TILE_SIZE;

			if (xCollision)
				*xCollision = true;
		}

		if (entity.getYState() == YState::Grounded && !m_map.touchingTile({ hitbox.x, hitbox.y + 2.f, hitbox.w, hitbox.h }, Tile::Property::Solid))
		{
			//std::cerr << "moving aside made the player fall" << std::endl;
			entity.setYState(YState::Falling);
		}
	}

	entity.setPosition(hitbox.x, hitbox.y);
}

void GameScene::moveEnemy(Enemy& enemy)
{
	bool xCollision = false;
	moveEntity(enemy, &xCollision);
	if (xCollision)
		enemy.toggleFacing();
}

void GameScene::setCameraOnPlayer(bool value)
{
	m_cameraOnPlayer = value;
}

void GameScene::updateCamera()
{
	if (m_cameraOnPlayer)
		placeCameraOnPlayer();
#if 1
	else if (m_readEvents)
	{
		moveCameraIfMouseOnEdge();
	}
#endif
}

void GameScene::placeCameraOnPlayer()
{
	sf::View currentView = m_window->getView();

	auto& playerBox = m_player->getHitbox();
	sf::Vector2f playerCenter(playerBox.x + playerBox.w / 2, playerBox.y + playerBox.h / 2);
	sf::Vector2f vecCenter = playerCenter - currentView.getCenter();

	// if (distance(vecCenter) > m_screenPadding)
	// {
		currentView.move(vecCenter);
		m_background.move(currentView.getCenter() - m_window->getView().getCenter());
		m_window->setView(currentView);

	// }
}

void GameScene::moveCamera(const sf::Vector2f& translation)
{
	auto view = m_window->getView();
	view.move(translation);
	m_window->setView(view);
}

void GameScene::moveCameraIfMouseOnEdge()
{
	// Move camera if mouse pointer is close to the edge of the map window
		// Regardless if map window is active or not
	auto windowSize = m_window->getSize();
	auto mousePositionInWindow = sf::Mouse::getPosition(*m_window);

	// Unactive map window?
	if (!m_window->hasFocus())
		return;

	// Mouse outside window? (even if active)
	if (mousePositionInWindow.x < 0 ||
		mousePositionInWindow.y < 0 ||
		(unsigned)mousePositionInWindow.x > windowSize.x ||
		(unsigned)mousePositionInWindow.y > windowSize.y)
		return;

	sf::Vector2f translation;

	// Right
	if (windowSize.x - mousePositionInWindow.x < m_mouseScreenPadding)
		translation.x = 10.f;

	// Left
	else if ((unsigned)mousePositionInWindow.x < m_mouseScreenPadding)
		translation.x = -10.f;

	// Bottom
	if (windowSize.y - mousePositionInWindow.y < m_mouseScreenPadding)
		translation.y = 10.f;

	// Top
	else if ((unsigned)mousePositionInWindow.y < m_mouseScreenPadding)
		translation.y = -10.f;

	// A camera translation may be required
	if (translation != sf::Vector2f())
		moveCamera(translation);
}

void GameScene::draw(sf::RenderTarget& target)
{
	target.draw(m_layers["backgroundLayer"]);
	target.draw(m_layers["mapLayer"]);
	target.draw(m_layers["mobsLayer"]);
	target.draw(m_layers["playerLayer"]);

	if (m_mapEditor)
		m_mapEditor->render();

	if (m_imguiEnabled)
    {
        ImGui::Text("Frame took %f FPS=%f", m_lastDt * 1000.f, 1 / m_lastDt);
		//ImGui::ShowDemoWindow();
    }
}
