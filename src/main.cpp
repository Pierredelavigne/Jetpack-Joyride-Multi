/*
** EPITECH PROJECT, 2023
** server
** File description:
** main
*/

#include "Exception/Exception.hpp"
#include "Server/Server.hpp"
#include "Args.hpp"

int main(int ac, char **av)
{
    try {
        if (ac != 7)
            throw Exception("Usage : ./serverJ2T3 -p <port> -g <gravity> -m <map>");
        Args args(av);
        Server server(args.getPort(), args.getGravity(), args.getMap());
        server.start();
    } catch (Exception &e) {
        std::cout << e.what() << std::endl;
        return (84);
    }
    return (0);
}
