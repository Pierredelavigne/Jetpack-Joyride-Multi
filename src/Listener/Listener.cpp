/*
** EPITECH PROJECT, 2023
** server
** File description:
** Listener
*/

#include "Listener/Listener.hpp"
#include "Exception/Exception.hpp"
#include <iostream>
#include <unistd.h>

Listener::~Listener() {}

bool Listener::createListenSocket()
{
    _server._listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_server._listenSocket == -1) {
        std::cout << "Failed to create listen socket" << std::endl;
        return false;
    }
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(_server.getPort());
    if (bind(_server._listenSocket, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cout << "Failed to bind listen socket" << std::endl;
        return false;
    }
    if (listen(_server._listenSocket, 3) < 0) {
        std::cout << "Failed to listen on socket" << std::endl;
        return false;
    }
    return true;
}

void Listener::disconnectClient(std::shared_ptr<Player>& player)
{
    _server.disconnectPlayer(player);
    std::cout << "Client disconnected" << std::endl;
}

void Listener::start()
{
    if (!createListenSocket()) {
        std::cout << "Failed to create listen socket" << std::endl;
        return;
    }
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
    while (_server.getPlayers().size() < 2 || !_server.playersReady()) {
        _server.handlePlayerCommands();
    }
    _server.setGameStatus(true);
    std::cout << "Game started" << std::endl;
    std::string response = "START\n";
    _server.sendToAllPlayers(response);
}
