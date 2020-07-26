#include "GameServer.hpp"

GameServer::GameServer() : m_ipAddress(sf::IpAddress::LocalHost), m_port(DEFAULT_PORT), m_state(ServerState::opened), m_udpPort(UDP_PORT)
{

}

GameServer::GameServer(sf::IpAddress &address, int port) : m_ipAddress(address), m_port(port), m_state(ServerState::opened), m_udpPort(UDP_PORT)
{

}

void GameServer::init(sf::IpAddress &address, int port)
{
    m_ipAddress = address;
    m_port = port;
    setState(ServerState::opened);
}

void GameServer::run()
{
    if(m_listener.listen(m_port, m_ipAddress) != sf::Socket::Done)
    {
        std::cout << "Server failed listening on [" << "" << "]..." << std::endl;
        shutdown();
        exit(EXIT_FAILURE);
    }
    std::cout << "Server listening on [" << m_ipAddress.toString() << ", " << m_port << "]..." << std::endl;
    m_selector.add(m_listener);

    while(m_state == ServerState::opened)
    {
        if(m_selector.wait())
        {
            if(m_selector.isReady(m_listener))
            {
                sf::TcpSocket* client = new sf::TcpSocket;
                if (m_listener.accept(*client) == sf::Socket::Done)
                {
                    // Add the new client to the clients list
                    GameServerClient gameClient;
                    gameClient.tcpSocket = client;
                    gameClient.port = m_udpPort;
                    gameClient.state = GameClientState::idle;
                    m_clients.push_back(gameClient);
                    m_udpPort += 2;
                    // Add the new client to the selector so that we will
                    // be notified when he sends something
                    m_selector.add(*client);
                    std::cout << "[" << client->getRemoteAddress() << ", " << client->getRemotePort() << "] connected..." << std::endl;

                }
                else
                {
                    // Error, we won't get a new connection, delete the socket
                    delete client;
                    std::cout << "Connection error..." << std::endl;
                }
            }
            else
            {
                for(std::list<GameServerClient>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
                {
                    sf::TcpSocket& client = *it->tcpSocket;
                    if (m_selector.isReady(client))
                    {
                        // The client has sent some data, we can receive it
                        sf::Packet packet;
                        std::string data;
                        if (client.receive(packet) == sf::Socket::Done)
                        {
                            packet >> data;
                            if(data == "play")
                            {
                                packet.clear();
                                packet << it->port + 1 << client.getRemoteAddress().toString();
                                client.send(packet);
                                it->udpSocket = new sf::UdpSocket();
                                it->udpSocket->bind(it->port, m_ipAddress);
                                it->state = GameClientState::playing;
                                std::cout << "[" << client.getRemoteAddress() << ", " << client.getRemotePort() << "] in game..." << std::endl;
                            }
                            else if(data == "quit")
                            {
                                delete it->udpSocket;
                                it->state = GameClientState::idle;
                                std::cout << "[" << client.getRemoteAddress() << ", " << client.getRemotePort() << "] wanna quit..." << std::endl;
                            }
                            else if(data == "update")
                            {
                                if(it->state == GameClientState::playing)
                                {
                                    sf::Packet packet;
                                    for(std::list<GameServerClient>::iterator user = m_clients.begin(); user != m_clients.end(); ++user)
                                    {
                                        packet << user->data;
                                    }
                                    it->udpSocket->send(packet, client.getRemoteAddress(), it->port);
                                }
                                else
                                {
                                    //error
                                }
                            }
                            else if(data == "disconnect")
                            {
                                m_selector.remove(client);
                                m_clients.erase(it++);
                                std::cout << "[" << client.getRemoteAddress() << ", " << client.getRemotePort() << "] disconnected..." << std::endl;
                                delete &client;
                            }
                        }
                        else
                        {
                            std::cout << "Package lost..." << std::endl;
                        }
                        
                    }
                }
            }
        }
    }
}

void GameServer::setState(ServerState state)
{
    m_state = state;
}

void GameServer::shutdown()
{
    setState(ServerState::closed);
    for(std::list<GameServerClient>::iterator it = m_clients.begin(); it != m_clients.end(); it++)
    {
        sf::TcpSocket* tcpClient = &*it->tcpSocket;
        delete tcpClient;
        sf::UdpSocket* udpClient = &*it->udpSocket;
        delete udpClient;
    }
}

GameServer::~GameServer()
{
    if(m_state != ServerState::closed)
    {
        shutdown();
    }
    std::cout << "A GameServer entity was destroyed" << std::endl;
}