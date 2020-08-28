#pragma once

#include "constantes.hpp"
#include <SFML/Network.hpp>

class GameClient
{
    public:
        GameClient();
        GameClient(sf::IpAddress &serverAddress, int tcpPort);
        ~GameClient();
        bool connect();
        void disconnect();
        void setState(ClientState state);
        std::string update();
        void play();
        void quit();
        std::string getServerData();
        void receiveData();
        void sendData(std::string data);

    private:
        sf::TcpSocket m_tcpSocket;
        sf::UdpSocket m_udpSocket;
        sf::IpAddress m_serverAddress;
        sf::IpAddress m_udpAddress;
        int m_state;
        int m_tcpPort;
        int m_udpPort;
        std::string m_data;
};
