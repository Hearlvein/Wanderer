#include "GameClient.hpp"

GameClient::GameClient() : m_serverAddress(sf::IpAddress::LocalHost), m_tcpPort(DEFAULT_PORT), m_state(ClientState::disconnected)
{
    std::cout << "A GameClient entity was created by default..." << std::endl;
}

GameClient::~GameClient()
{
    if(m_state == ClientState::connected)
        disconnect();
    std::cout << "A GameClient entity was destroyed..." << std::endl;
}

GameClient::GameClient(sf::IpAddress &serverAddress, int tcpPort) : m_serverAddress(serverAddress), m_tcpPort(tcpPort), m_state(ClientState::disconnected)
{
    std::cout << "A GameClient entity was created..." << std::endl;
}

bool GameClient::connect()
{
    if(m_tcpSocket.connect(m_serverAddress, m_tcpPort) != sf::Socket::Done)
    {
        std::cout << "Failed to connect [" << m_serverAddress.toString() << ", " << m_tcpPort << "]..." << std::endl;
        return false;
    }
    setState(ClientState::connected);
    std::cout << "Connected to [" << m_serverAddress.toString() << ", " << m_tcpPort << "]..." << std::endl;
    return true;
}

void GameClient::disconnect()
{
    sf::Packet packet;
    packet << "disconnect";
    m_tcpSocket.send(packet);
    m_tcpSocket.disconnect();
    setState(ClientState::disconnected);
    std::cout << "Disconnected from [" << m_serverAddress.toString() << ", " << m_tcpPort << "]..." << std::endl;
}

void GameClient::setState(ClientState state)
{
    m_state = state;
}

std::string GameClient::update()
{
    sf::Packet packet;
    packet << "update";
    if(m_tcpSocket.send(packet) != sf::Socket::Done)
    {
        std::cout << "Error : asking for update..." << std::endl;
        return "error";
    }
    return "ok";
}

void GameClient::quit()
{
    sf::Packet packet;
    packet << "update";
    if(m_tcpSocket.send(packet) != sf::Socket::Done)
    {
        std::cout << "Error : asking for leaving..." << std::endl;
    }
}

void GameClient::play()
{
    sf::Packet packet;
    packet << "play";
    if(m_tcpSocket.send(packet) != sf::Socket::Done)
    {
        std::cout << "Error : asking for playing..." << std::endl;
    }
    else
    {
        std::string udpAddress;
        packet.clear();
        m_tcpSocket.receive(packet);
        packet >> m_udpPort >> udpAddress;
        m_udpAddress = sf::IpAddress(udpAddress);
        m_udpSocket.bind(m_udpPort, m_udpAddress);
        std::cout << "Creating UDP connection [" << udpAddress << m_udpPort << "]..." << std::endl;
        m_udpPort--;
        std::cout << "Server launching game..." << std::endl;
    }
    
}

std::string GameClient::getServerData()
{
    return m_data;
}

void GameClient::receiveData()
{
    sf::Packet packet;
    sf::IpAddress sender;
    short unsigned int sender_port;
    std::string data;
    if(m_udpSocket.receive(packet, sender, sender_port) != sf::Socket::Done)
    {
        std::cout << "Error : receving data..." << std::endl;
    }
    else
    {
        if(!(packet >> data))
        {
            std::cout << "Error : decoding data..." << std::endl;
        }
        else
        {
            packet >> m_data;
        }
    }
    
}

void GameClient::sendData(std::string data)
{
    sf::Packet packet;
    m_udpSocket.send(packet, m_tcpSocket.getRemoteAddress(), m_udpPort);
}
