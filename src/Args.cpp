/*
** EPITECH PROJECT, 2023
** server
** File description:
** Args
*/

#include "Args.hpp"
#include "Exception/Exception.hpp"

bool isNumeric(const std::string& str) {
    return str.find_first_not_of("0123456789") == std::string::npos;
}

Args::Args(char **av)
{
    if (std::string(av[1]) != "-p" || std::string(av[3]) != "-g" || std::string(av[5]) != "-m")
        throw Exception("Usage : ./serverJ2T3 -p <port> -g <gravity> -m <map>");
    if (!isNumeric(av[2]) || !isNumeric(av[4])) {
    throw Exception("Port and gravity must be numeric");
    }
    _port = av[2];
    _gravity = av[4];
    std::ifstream file(av[6]);
    if (!file.is_open())
        throw Exception("Map file not found");
    std::string line;
    while (std::getline(file, line))
        _map += line + "\n";
    file.close();
    _map = _map.substr(0, _map.size() - 1);
}
