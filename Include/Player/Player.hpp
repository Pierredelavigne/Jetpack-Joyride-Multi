/*
** EPITECH PROJECT, 2023
** server
** File description:
** Player
*/

#pragma once

class Player {
    public:
        Player(int id, int clientSocket) : _id(id), _clientSocket(clientSocket), _x(0), _y(0), _score(0), _fireState(false), _ready(false) {};
        ~Player() {close(_clientSocket);};

        int getId() const { return _id; };
        int getClientSocket() { return _clientSocket; };
        int getX() const { return _x; };
        int getY() const { return _y; };
        int getScore() const { return _score; };
        bool getFireState() const { return _fireState; };
        void scoresUp() {_score++; };

        void move(int x, int y) { _x += x; _y += y; };
        void setFireState(bool fireState) { _fireState = fireState; };
        void setReady(bool ready) { _ready = ready; };
        bool isReady() const { return _ready; };
        bool isAlive() const { return _isAlive; };
        void setAlive(bool alive) { _isAlive = alive; };

    protected:
    private:
        int _id;
        int _clientSocket;
        int _x;
        int _y;
        int _score;
        bool _fireState;
        bool _ready;
        bool _isAlive = true;
};
