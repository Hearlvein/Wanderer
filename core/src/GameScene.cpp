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
	LINE_TRACK
	m_layers["backgroundLayer"].addObject(m_bgMgr.getDrawable());

	LINE_TRACK

	if (!m_texture.loadFromFile(texturesPath + "tileset.png"))
		std::cout << "Tileset not loaded!" << std::endl;
	m_texture.setSmooth(true);

	m_map.load(levelsPath + "level.txt");
	m_layers["mapLayer"].addObject(&m_map);

	LINE_TRACK

	m_player.setTexture(m_texture);
	LINE_TRACK
	m_player.setPosition(100.f, 50.f);

	LINE_TRACK

	m_player.addTextureRectAndHitbox("right", sf::IntRect(0, 50, 50, 100), sf::FloatRect(0.f, 50.f, 50.f, 100.f));
	m_player.addTextureRectAndHitbox("right", sf::IntRect(50, 50, 50, 100), sf::FloatRect(50.f, 50.f, 50.f, 100.f));
	m_player.addTextureRectAndHitbox("left", sf::IntRect(100, 50, 50, 100), sf::FloatRect(100.f, 50.f, 50.f, 100.f));
	m_player.addTextureRectAndHitbox("left", sf::IntRect(150, 50, 50, 100), sf::FloatRect(150.f, 50.f, 50.f, 100.f));
	m_player.addTextureRectAndHitbox("idleRight", sf::IntRect(0, 50, 50, 100), sf::FloatRect(0.f, 50.f, 50.f, 100.f));
	m_player.addTextureRectAndHitbox("idleLeft", sf::IntRect(100, 50, 50, 100), sf::FloatRect(100.f, 50.f, 50.f, 100.f));
	m_player.addTextureRectAndHitbox("climbing", sf::IntRect(200, 50, 50, 100), sf::FloatRect(200.f, 50.f, 50.f, 100.f));

	m_player.setCurrentSeq("idleRight");
	m_layers["playerLayer"].addObject(&m_player);

	LINE_TRACK

	/*for (std::size_t i = 0; i < 2; ++i)
	{
		m_enemiesList.push_back(Enemy());
		Enemy& enemy = m_enemiesList.back();
		enemy.setTexture(m_texture);
		enemy.createSeq("right", sf::Vector2i(0, 150), 2);
		enemy.createSeq("left", sf::Vector2i(100, 150), 2);
		enemy.createSeq("idleRight", sf::Vector2i(0, 150), 1);
		enemy.createSeq("idleLeft", sf::Vector2i(100, 150), 1);
		enemy.setCurrentSeq("idleRight");
		enemy.setWalkingState(Beginning);
		enemy.setPosition(static_cast<float>(i + 1) * 500.f, 150.f);
		m_layers["mobsLayer"].addObject(&enemy);
	}*/
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
        m_player.setFacing(Right);
        m_player.setWalkingState(Beginning);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
    	m_player.setFacing(Left);
        m_player.setWalkingState(Beginning);
    }
    else
    {
    	m_player.setWalkingState(End);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    	m_player.setYState(Jumping);
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && m_map.touchingTile(m_player.getCurrentHitbox(), Ladder))
    {
    	m_player.setYState(Climbing);
    	m_player.setClimbingDirection(Up);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && m_map.touchingTile(m_player.getCurrentHitbox(), Ladder))
    {
    	m_player.setYState(Climbing);
    	m_player.setClimbingDirection(Down);
    }
    else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && m_map.touchingTile(m_player.getCurrentHitbox(), Ladder))
    	m_player.setClimbingDirection(None);

    /*if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
    	addFireball();*/
}

void GameScene::update(float dt)
{

	m_dt = dt;

	// Entity collisions
	for (auto enemyIt = m_enemiesList.begin(); enemyIt != m_enemiesList.end(); ++enemyIt)
	{
		if (m_player.getCurrentHitbox().x < enemyIt->getCurrentHitbox().x + enemyIt->getCurrentHitbox().w &&
			m_player.getCurrentHitbox().x + m_player.getCurrentHitbox().w > enemyIt->getCurrentHitbox().x &&
			m_player.getCurrentHitbox().y < enemyIt->getCurrentHitbox().y + enemyIt->getCurrentHitbox().h &&
			m_player.getCurrentHitbox().y + m_player.getCurrentHitbox().h > enemyIt->getCurrentHitbox().y)
		{
			m_player.takeDamage(20);
			m_player.setIsInvicible(true, 2.f);
		}

		/*for (auto fbIt = m_fireballs.begin(); fbIt != m_fireballs.end(); ++fbIt)
		{
			if (fbIt->getPosition().x < enemyIt->getPosition().x + enemyIt->getSize().x &&
				fbIt->getPosition().x + fbIt->getSize().x > enemyIt->getPosition().x &&
				fbIt->getPosition().y < enemyIt->getPosition().y + enemyIt->getSize().y &&
				fbIt->getPosition().y + fbIt->getSize().y > enemyIt->getPosition().y)
			{
				enemyIt->takeDamage(20);

				if (!enemyIt->isAlive())
					enemyIt = m_enemiesList.erase(enemyIt);

				fbIt = m_fireballs.erase(fbIt);
			}
		}*/
	}

	m_player.update(dt);
	updateClimbingState(m_player);
	moveEntity(m_player);

	for (auto& enemy : m_enemiesList)
    {
        enemy.update(dt);
        updateClimbingState(enemy);
        moveEnemy(enemy);
    }

    /*for (auto fb = m_fireballs.begin(); fb != m_fireballs.end(); ++fb)
    {
    	fb->update(dt);
    	if (!moveFireball(fb))
    		break;
    }*/

    moveCamera();
}

void GameScene::updateClimbingState(MovingGameObject& entity)
{
	if (entity.getYState() == Climbing && !m_map.touchingTile(m_player.getCurrentHitbox(), Ladder))
		entity.setYState(Falling);
}

void GameScene::moveEntity(MovingGameObject& entity, bool* xCollision)
{
	Box hitbox = entity.getCurrentHitbox();

	float dx = entity.getMovement().x;
	float dy = entity.getMovement().y;

	// Y checking
	if (dy != 0.f)
	{
		if (std::abs(dy) >= TILE_SIZE)
        {
            std::cout << "dy=" << dy;
            dy = (TILE_SIZE - 1.f) * ((dy < 0) ? -1 : 1);
            std::cout << " -> " << dy << std::endl;
        }

		if (m_map.isEmptyArea({hitbox.x, hitbox.y+dy, hitbox.w, hitbox.h}))
			hitbox.y += dy;
		else
		{
			if (entity.getYState() == Falling)
			{
				hitbox.y = std::ceil((hitbox.y+hitbox.h-1.f) / TILE_SIZE) * TILE_SIZE - hitbox.h;
				entity.setYState(Grounded);
			}
			else if (entity.getYState() == Jumping)
			{
                std::cout << "too high jump" << std::endl;
                entity.setYState(Falling);
			}
		}
	}

	// X checking
	if (dx != 0.f)
	{
		if (std::abs(dx) >= TILE_SIZE)
        {
            std::cout << "dx=" << dx;
            dx = (TILE_SIZE - 1.f) * ((dx < 0) ? -1 : 1);
            std::cout << " -> " << dx << std::endl;
        }

        if (m_map.isEmptyArea({hitbox.x+dx, hitbox.y, hitbox.w, hitbox.h}))
			hitbox.x += dx;
		else
		{
			if (dx > 0)
				hitbox.x = std::ceil((hitbox.x+hitbox.w-1.f) / TILE_SIZE) * TILE_SIZE - hitbox.w;
			else
                hitbox.x = std::floor(hitbox.x / TILE_SIZE) * TILE_SIZE;

			if (xCollision)
                *xCollision = true;
            //entity.setWalkingState(Idle);
		}

		if (entity.getYState() != Jumping && m_map.isEmptyArea({hitbox.x, hitbox.y+2.f, hitbox.w, hitbox.h}))
			entity.setYState(Falling);
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

/*bool GameScene::moveFireball(std::list<Fireball>::iterator& fb)
{
	float x = fb->getPosition().x;
	float y = fb->getPosition().y;
	float w = fb->getSize().x;
	float h = fb->getSize().y;

	float dx = fb->getMovement().x;

	// X checking
	if (dx != 0.f)
	{
		if (m_map.isEmptyArea(x+dx, y, w, h))
		{
			x += dx;
			fb->setPosition(x, y);
		}
		else
		{
			fb = m_fireballs.erase(fb);
			return false;
		}
	}

	return true;
}*/

void GameScene::moveCamera()
{
	sf::View currentView = m_window->getView();

	auto tR = m_player.getCurrentTextureRect();

	float moveRight = m_window->mapCoordsToPixel(sf::Vector2f(static_cast<float>(tR.left + tR.width), 0.f), currentView).x
		- m_window->mapCoordsToPixel(sf::Vector2f(m_screenPadding, 0.f), currentView).x;
	std::cout << "move right=" << moveRight << std::endl;
	if (moveRight > 0.f)
		currentView.move(moveRight, 0.f);

    m_bgMgr.update(currentView.getCenter() - m_window->getView().getCenter());

    m_window->setView(currentView);
}

/*void GameScene::addFireball()
{
	m_fireballs.push_back(Fireball());
	Fireball& fb = m_fireballs.back();
	fb.setSize(25.f, 25.f);
	fb.setTexture(m_texture);
	fb.setTexturePosition(sf::Vector2i(0, 250));
	fb.setPosition(m_player.getSize().x / 2 + m_player.getPosition().x, m_player.getSize().y / 2 + m_player.getPosition().y);
	fb.setVelocity(100.f, 0.f);

	if (m_player.getFacing() == Right)
		fb.setDirection(Right);
	else
		fb.setDirection(Left);

	m_layers["fireballsLayers"].addObject(&fb);

	// sf::sleep(sf::seconds(0.2f));
}*/

void GameScene::draw(sf::RenderTarget& target)
{
	DEBUG_FC
    target.draw(m_layers["backgroundLayer"]);
    target.draw(m_layers["mapLayer"]);
    // target.draw(m_layers["mobsLayer"]);
    target.draw(m_layers["playerLayer"]);
    target.draw(m_layers["fireballsLayers"]);

    if (m_imguiEnabled)
    	ImGui::Text("Frame took %f FPS=%f", m_dt, 1 / m_dt);
}

