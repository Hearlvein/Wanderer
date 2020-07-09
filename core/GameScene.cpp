#include <iostream>
#include <cmath>
#include <imgui-SFML.h>
#include <imgui.h>
#include "GameScene.hpp"
#include "Constants.hpp"
#include "util.hpp"
#include "debug.hpp"

GameScene::GameScene(sf::RenderWindow* window)
	: Scene(window), m_map(&m_texture)
{
	m_layers["backgroundLayer"].addObject(m_bgMgr.getDrawable());

	if (!m_texture.loadFromFile(texturesPath + "tileset.png"))
		std::cout << "Tileset not loaded!" << std::endl;
	m_texture.setSmooth(true);

	m_map.load(levelsPath + "level.txt");
	m_layers["mapLayer"].addObject(&m_map);

	m_player.setTexture(m_texture);
	m_player.setPosition(100.f, 50.f);
	m_layers["playerLayer"].addObject(&m_player);


	for (std::size_t i = 0; i < 2; ++i)
	{
		// Registers in enemy list
		m_enemiesList.push_back(Enemy());
		Enemy& enemy = m_enemiesList.back();

		enemy.setTexture(m_texture);
		enemy.setPosition(static_cast<float>(i + 1) * 500.f, 150.f);

		// Registers in mobs layer
		m_layers["mobsLayer"].addObject(&enemy);
	}
}

GameScene::~GameScene()
{

}

void GameScene::handleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::Closed)
    	m_window->close();
}

void GameScene::checkInput()
{
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
		// no updateClimbingState because entity can't climb ladders
        moveEnemy(enemy);
    }

    moveCamera();
}

void GameScene::updateClimbingState(MovingCharacter& entity)
{
	if (entity.getYState() == YState::Climbing && !m_map.touchingTile(m_player.getHitbox(), Ladder))
		entity.setYState(YState::Falling);
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
		m_bgMgr.update(currentView.getCenter() - m_window->getView().getCenter());
		m_window->setView(currentView);

	// }
}

void GameScene::draw(sf::RenderTarget& target)
{
    target.draw(m_layers["backgroundLayer"]);
    target.draw(m_layers["mapLayer"]);
    target.draw(m_layers["mobsLayer"]);
    target.draw(m_layers["playerLayer"]);

    if (m_imguiEnabled)
    	ImGui::Text("Frame took %f FPS=%f", m_lastDt, 1 / m_lastDt);
}