##
## EPITECH PROJECT, 2023
## Makefile
## File description:
## Makefile
##

NAME = serverJ2T3

SRC = 	src/main.cpp	\
		src/Server/Server.cpp	\
		src/Listener/Listener.cpp	\
		src/Args.cpp	\

OBJ = $(SRC:.cpp=.o)

CXXFLAGS  = -std=c++17 -I./Include -g -lsfml-graphics -lsfml-window -lsfml-system

all    :    $(OBJ)
		g++ -o $(NAME) $(OBJ) $(CXXFLAGS)

clean    :
		rm -f $(OBJ)

fclean    :    clean
		rm -f $(NAME)

re    :    fclean all

.PHONY    :    all clean fclean re
