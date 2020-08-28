#pragma once
#include "constantes.hpp"
#include "GameServerClient.hpp"
#include <SFML/Network.hpp>
#include <SFML/System.hpp>

class GameServer
{
    public:
        GameServer();
        GameServer(sf::IpAddress &address, int port);
        ~GameServer();
        void init(sf::IpAddress &address, int port);
        void run();
        void shutdown();
        void setState(ServerState state);
        
        
    private:
        sf::IpAddress m_ipAddress;
        int m_port;
        int m_udpPort;
        sf::TcpListener m_listener;
        int m_state;
        std::list<GameServerClient> m_clients;
        sf::SocketSelector m_selector;
};