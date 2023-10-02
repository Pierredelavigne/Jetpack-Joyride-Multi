/*
** EPITECH PROJECT, 2023
** server
** File description:
** Server
*/

#pragma once
#include <bits/stdc++.h>
#include "Player/Player.hpp"
#include <SFML/System.hpp>

class Server {
    public:
        Server(std::string const &port, std::string const &gravity, std::string const &map) : _port(port), _gravity(gravity), _map(map) {};
        ~Server();

        void start();

        int getPort() const { return std::stoi(_port); };
        std::string getGravity() const { return _gravity; };
        std::string getMap() const { return _map; };
        bool getGameStatus() const { return _gameStarted; };
        void setGameStatus(bool status) { _gameStarted = status; };
        std::vector<std::shared_ptr<Player>> getPlayers() const { return _players; };
        void addPlayer(std::shared_ptr<Player> player) { _players.push_back(player); };
        void disconnectPlayer(std::shared_ptr<Player> player);
        void handleCommands(std::shared_ptr<Player>& player, char* buffer);
        int generatePlayerId();
        void handlePlayerCommands();
        void sendToAllPlayers(const std::string& message);
        bool playersReady();
        fd_set _readSet;
        int _listenSocket;

    protected:
    private:
        std::string _port;
        std::string _gravity;
        std::string _map;
        bool _gameStarted = false;
        std::vector<std::shared_ptr<Player>> _players;

        void startGameLoop();
        void checkEndGame();
        void acceptNewConnection();

        void endGame();
        void updatePlayerStates();
        bool handleCollisions(std::shared_ptr<Player> player);
};
