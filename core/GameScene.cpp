#include <iostream>
#include <cmath>
#include <imgui-SFML.h>
#include <imgui.h>
#include <functional>
#include "GameScene.hpp"
#include "Constants.hpp"
#include "util.hpp"
#include "debug.hpp"

GameScene::GameScene(sf::RenderWindow* window)
	: Scene(window)
{
	// ------------------- textures -------------------
	m_tileset.loadFromFile(texturesPath + "tileset.png");
	m_backgroundTexture.loadFromFile(texturesPath + "background.png");

	// ------------------- background -------------------
	m_background.getSprite()->setTexture(m_backgroundTexture);
	m_layers["backgroundLayer"].addObject(m_background.getDrawable());


	// ------------------- map -------------------
	m_map.setTexture(m_tileset);
	m_map.load(levelsPath + "level.txt", [&](const std::string& n, const sf::Vector2f& p)
	{
		addEntity(n, p);
	});
	m_layers["mapLayer"].addObject(&m_map);
}

GameScene::~GameScene()
{

}

void GameScene::handleEvent(const sf::Event& event)
{
	if (event.type == sf::Event::Closed)
    {
        if (m_mapEditor)
            m_mapEditor->close();

        m_window->close();
    }
}

void GameScene::checkInput()
{
	// map editor
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
	{
		if (m_mapEditor)
			m_mapEditor.reset();
		else
			m_mapEditor = std::make_unique<MapEditor>(
                *m_window,
                m_map,
                std::map<char, const GameObject&>{ {'p', m_player} },
                m_tileset);

		sf::sleep(sf::milliseconds(100));	// prevent spamming
	}

	if (m_mapEditor)
		m_mapEditor->checkInput();

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		m_player.setFacing(Direction::Right);
		m_player.setWalkingState(WalkingState::Beginning);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		m_player.setFacing(Direction::Left);
		m_player.setWalkingState(WalkingState::Beginning);
	}
	else
		m_player.setWalkingState(WalkingState::End);


	// Check for jump
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		m_player.setYState(YState::Jumping);

	// Check for ladder
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && m_map.touchingTile(m_player.getHitbox(), Ladder))
	{
		m_player.setYState(YState::Climbing);
		m_player.setClimbingDirection(Direction::Up);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && m_map.touchingTile(m_player.getHitbox(), Ladder))
	{
		m_player.setYState(YState::Climbing);
		m_player.setClimbingDirection(Direction::Down);
	}
	else if (m_map.touchingTile(m_player.getHitbox(), Ladder))
		m_player.setClimbingDirection(Direction::None);
}

void GameScene::update(float dt)
{
	m_lastDt = dt;

	// Enemy collision
	if (!m_player.isInvicible())
	{
		for (auto enemyIt = m_enemiesList.begin(); enemyIt != m_enemiesList.end(); ++enemyIt)
		{
			if (boxesOverlapping(enemyIt->getHitbox(), m_player.getHitbox()))
			{
				m_player.takeDamage(20);
				m_player.setIsInvicible(true, 2.f);
			}
		}
	}

	// internal player update
	m_player.update(dt);

	// external player update
	updateClimbingState(m_player);
	moveEntity(m_player);

	// enemies update
	for (auto& enemy : m_enemiesList)
	{
		enemy.update(dt);
		// no updateClimbingState because entities can't climb ladders
		moveEnemy(enemy);
	}

	moveCamera();
}

void GameScene::updateClimbingState(MovingCharacter& entity)
{
	if (entity.getYState() == YState::Climbing && !m_map.touchingTile(m_player.getHitbox(), Ladder))
		entity.setYState(YState::Falling);
}

void GameScene::addEntity(const std::string& name, const sf::Vector2f& position)
{
	if (name == "player")
	{
		m_player.setPosition(position.x, position.y);
		m_player.setTexture(m_tileset);
		m_layers["playerLayer"].addObject(&m_player);
	}
	else if (name == "enemy")
	{
		// Register in enemy list
		m_enemiesList.push_back(Enemy());
		Enemy& enemy = m_enemiesList.back();

		enemy.setTexture(m_tileset);
		enemy.setPosition(position.x, position.y);
		m_layers["mobsLayer"].addObject(&enemy);
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

		if (!m_map.touchingTile({hitbox.x, hitbox.y+dy, hitbox.w, hitbox.h}, Solid))
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

		if (!m_map.touchingTile({hitbox.x+dx, hitbox.y, hitbox.w, hitbox.h}, Solid))
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

		if (entity.getYState() == YState::Grounded && !m_map.touchingTile({ hitbox.x, hitbox.y + 2.f, hitbox.w, hitbox.h }, Solid))
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

void GameScene::moveCamera()
{
	sf::View currentView = m_window->getView();

	sf::Vector2f playerCenter(m_player.getPosition().x + m_player.getCurrentTextureRect().width / 2, m_player.getPosition().y + m_player.getCurrentTextureRect().height / 2);
	sf::Vector2f vecCenter = playerCenter - currentView.getCenter();
	// if (distance(vecCenter) > m_screenPadding)
	// {
		currentView.move(vecCenter);
		m_background.update(currentView.getCenter() - m_window->getView().getCenter());
		m_window->setView(currentView);

	// }
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
		ImGui::Text("Frame took %f FPS=%f", m_lastDt * 1000.f, 1 / m_lastDt);
}
