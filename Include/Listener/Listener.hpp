/*
** EPITECH PROJECT, 2023
** server
** File description:
** Listener
*/

#pragma once
#include "Server/Server.hpp"
#include "netinet/in.h"
#include "Player/Player.hpp"
#include <arpa/inet.h>

#define DEBUG_MODE true

class Listener {
public:
    Listener(Server& server) : _server(server) {}
    ~Listener();

    void start();
    void disconnectClient(std::shared_ptr<Player>& player);

private:
    Server& _server;

    bool createListenSocket();
    bool areAllPlayersReady() const;
};
