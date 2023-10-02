/*
** EPITECH PROJECT, 2023
** server
** File description:
** Args
*/

#pragma once
#include <bits/stdc++.h>

class Args {
    public:
        Args(char **av);
        ~Args() = default;

        std::string getPort() const { return _port; };
        std::string getGravity() const { return _gravity; };
        std::string getMap() const { return _map; };

    protected:
    private:
        std::string _port;
        std::string _gravity;
        std::string _map;
};
