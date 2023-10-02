/*
** EPITECH PROJECT, 2023
** server
** File description:
** Exception
*/

#pragma once
#include <exception>
#include <string>

class Exception : public std::exception {
    public:
        Exception(std::string const &message) : _message(message) {};
        ~Exception() = default;

        const char *what() const noexcept override { return _message.c_str(); };

    protected:
    private:
        std::string _message;
};
