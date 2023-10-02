/*
** EPITECH PROJECT, 2023
** server
** File description:
** Server
*/

#define DEBUG_MODE true

#include "Server/Server.hpp"
#include "Listener/Listener.hpp"


Server::~Server()
{
    if (DEBUG_MODE)
        std::cout << "Server is stopping..." << std::endl;
}

void Server::start()
{
    if (DEBUG_MODE)
        std::cout << "Server is starting..." << "\nport: " << _port << "\nmap:\n" << _map << "\ngravity: " << _gravity << std::endl;
    Listener listener(*this);
    listener.start();
    startGameLoop();
}

void Server::handlePlayerCommands()
{
    int maxSocket = 0;
    FD_ZERO(&_readSet);
    FD_SET(_listenSocket, &_readSet);
    maxSocket = std::max(maxSocket, _listenSocket);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
    for (auto& player : getPlayers()) {
        int clientSocket = player->getClientSocket();
        FD_SET(clientSocket, &_readSet);
        maxSocket = std::max(maxSocket, clientSocket);
    }
    int activity = select(maxSocket + 1, &_readSet, NULL, NULL, &timeout);
    if (activity < 0) {
        std::cout << "Select error" << std::endl;
        return;
    }
    if (FD_ISSET(_listenSocket, &_readSet) && getPlayers().size() < 2) {
        acceptNewConnection();
    }
    for (auto& player : getPlayers()) {
        int clientSocket = player->getClientSocket();
        if (FD_ISSET(clientSocket, &_readSet)) {
            constexpr int bufferSize = 1024;
            char buffer[bufferSize];
            memset(buffer, 0, bufferSize);

            int bytesRead = recv(clientSocket, buffer, bufferSize - 1, 0);
            if (bytesRead < 0) {
                if (errno == EWOULDBLOCK || errno == EAGAIN) {
                    continue;
                } else {
                    disconnectPlayer(player);
                    continue;
                }
            } else if (bytesRead == 0) {
                disconnectPlayer(player);
                continue;
            }
            handleCommands(player, buffer);
        }
    }
}

std::vector<std::string> parseCommands(const std::string& input)
{
    std::vector<std::string> commands;
    std::istringstream iss(input);
    std::string line;

    while (std::getline(iss, line, '\n')) {
        commands.push_back(line);
    }

    return commands;
}

std::string getMapHeight(const std::string& map)
{
    int height = 0;
    std::istringstream iss(map);
    std::string line;

    while (std::getline(iss, line, '\n')) {
        height++;
    }

    return std::to_string(height);
}

std::string getMapInLine(const std::string& map)
{
    std::string mapInLine = map;
    mapInLine.erase(std::remove(mapInLine.begin(), mapInLine.end(), '\n'), mapInLine.end());
    return mapInLine;
}

std::string getMapWidth(const std::string& map)
{
    int width = 0;
    std::istringstream iss(map);
    std::string line;

    std::getline(iss, line, '\n');
    width = line.length();

    return std::to_string(width);
}

void Server::handleCommands(std::shared_ptr<Player>& player, char* buffer)
{
    std::string input(buffer);
    std::vector<std::string> commands = parseCommands(input);
    for (const std::string& command : commands) {
        if (command == "ID") {
            int playerId = player->getId();
            std::string response = "ID " + std::to_string(playerId) + "\n";
            send(player->getClientSocket(), response.c_str(), response.length(), 0);
        } else if (command == "READY") {
            std::cout << "Player " << player->getId() << " is ready" << std::endl;
            player->setReady(true);
        } else if (command == "MAP") {
            std::string response = "MAP " + getGravity() + " " + getMapWidth(getMap()) + " " + getMapHeight(getMap()) + " " + getMapInLine(getMap()) + "\n";
            send(player->getClientSocket(), response.c_str(), response.length(), 0);
        } else if (command.substr(0, 4) == "FIRE") {
            if (command.length() >= 6) {
                std::string state = command.substr(5);
                if (!state.empty()) {
                    int jetpackState = std::stoi(state);
                    player->setFireState(jetpackState == 1);
                }
            }
        }
    }
}

int Server::generatePlayerId()
{
    int playerId = 0;
    for (const auto& player : _players) {
        playerId = std::max(playerId, player->getId());
    }
    return playerId + 1;
}

void Server::disconnectPlayer(std::shared_ptr<Player> player)
{
    int clientSocket = player->getClientSocket();
    close(clientSocket);
    _players.erase(std::remove(_players.begin(), _players.end(), player), _players.end());
    std::cout << "Player " << player->getId() << " disconnected" << std::endl;
}

void Server::sendToAllPlayers(const std::string& message)
{
    for (const auto& player : _players) {
        send(player->getClientSocket(), message.c_str(), message.length(), 0);
    }
}

void Server::updatePlayerStates()
{
    for (const auto& player : _players) {
        int xSpeed = 1;
        int ySpeed = player->getFireState() == 1 ? -std::stoi(_gravity) : std::stoi(_gravity);
        if (player->getFireState() && player->getY() < 1) {
            ySpeed = 0;
        }
        if (player->getX() > std::stoi(getMapWidth(getMap())) - 2) {
            std::cout << "Player " << player->getId() << " won" << std::endl;
            endGame();
        }
        if (player->getY() >= std::stoi(getMapHeight(getMap())) && player->getFireState() == 0) {
            ySpeed = 0;
        }
        player->move(xSpeed, ySpeed);
        int y = std::stoi(getMapHeight(getMap())) - player->getY();
        handleCollisions(player);
        std::string response = "PLAYER " + std::to_string(player->getId()) + " " + std::to_string(player->getX()) + " " + std::to_string(y) + " " + std::to_string(player->getScore()) + " " + std::to_string(player->getFireState()) + "\n";
        sendToAllPlayers(response);
    }
}

void Server::startGameLoop()
{
    const sf::Time frameDuration = sf::milliseconds(1000.0 / 5.9);
    sf::Clock frameClock;
    for (const auto& player : _players)
        player->move(0, std::stoi(getMapHeight(getMap())) / 2);
    while (_gameStarted) {
        sf::Time elapsed = frameClock.restart();
        handlePlayerCommands();
        updatePlayerStates();
        checkEndGame();
        sf::Time frameElapsed = frameClock.getElapsedTime();
        sf::Time sleepTime = frameDuration - frameElapsed;

        if (sleepTime > sf::Time::Zero) {
            sf::sleep(sleepTime);
        }
    }
}

std::string concatenateMap(const std::vector<std::string>& mapLines)
{
    std::string result;
    for (const auto& line : mapLines) {
        result += line;
        result += '\n';
    }
    return result;
}

bool Server::handleCollisions(std::shared_ptr<Player> player)
{
    int playerX = player->getX();
    int playerY = player->getY();
    std::istringstream iss(_map);
    std::string mapLine;
    int lineNumber = 1;

    while (std::getline(iss, mapLine, '\n')) {
        if (lineNumber == playerY) {
            char tile = mapLine[playerX];
            if (tile == 'e') {
                player->setAlive(false);
                return true;
            }
            if (tile == 'c') {
                player->scoresUp();
                std::string response = "COIN " + std::to_string(player->getId()) + " " + std::to_string(playerX) + " " + std::to_string(lineNumber - 1) + "\n";
                std::cout << response << std::endl;
                sendToAllPlayers(response);
                return true;
            }
        }
        lineNumber++;
    }
    return false;
}

bool Server::playersReady()
{
    for (const auto& player : _players) {
        if (!player->isReady())
            return false;
    }
    return true;
}

void Server::endGame()
{
    std::cout << "Game ended" << std::endl;
    int winnerId = 0;

    for (const auto& player : _players) {
        if (player->getScore() > winnerId && player->isAlive())
            winnerId = player->getId();
    }
    std::string response = "FINISH " + std::to_string(winnerId) + "\n";
    _gameStarted = false;
    sendToAllPlayers(response);
    sleep(5);
    for (auto player : _players) {
        close(player->getClientSocket());
        std::cout << "Player " << player->getId() << " disconnected" << std::endl;
    }
    close(_listenSocket);
    std::cout << "closing server" << std::endl;
    exit(0);
}

void Server::checkEndGame()
{
    int alivePlayers = 0;
    for (const auto& player : _players) {
        if (player->isAlive())
            alivePlayers++;
    }
    if (alivePlayers <= 1) {
        std::cout << "dead" << std::endl;
        endGame();
    }
}

void Server::acceptNewConnection()
{
    sockaddr_in clientAddress;
    socklen_t addressLength = sizeof(clientAddress);
    int newSocket = accept(_listenSocket, (struct sockaddr*)&clientAddress, &addressLength);
    if (newSocket < 0) {
        std::cout << "Failed to accept new connection" << std::endl;
        return;
    }

    int playerId = generatePlayerId();

    std::shared_ptr<Player> newPlayer = std::make_shared<Player>(playerId, newSocket);
    addPlayer(newPlayer);
    if (DEBUG_MODE)
        std::cout << "New connection accepted (Player ID: " << playerId << ")" << std::endl;
}
