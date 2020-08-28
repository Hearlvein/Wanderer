#pragma once
#include "constantes.hpp"
#include <SFML/Network.hpp>


typedef struct GameServerClient
{
    sf::TcpSocket *tcpSocket;
    sf::UdpSocket *udpSocket;
    std::string data;
    int port;
    int state;
}GameServerClient;