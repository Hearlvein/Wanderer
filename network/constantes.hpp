#pragma once
#include <iostream>
#include <string>
#include <list>

#define DEFAULT_PORT 53000
#define CLIENT_LIMIT 2
#define UDP_PORT 55000

enum ServerState
{
    closed,
    opened,
    full
};

enum GameClientState
{
    idle,
    playing
};

enum ClientState
{
    connected,
    disconnected
};