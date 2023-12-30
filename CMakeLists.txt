cmake_minimum_required(VERSION 3.26)
project(SchiffeVersenken_Projekt_)

set(CMAKE_CXX_STANDARD 17)

add_executable(SchiffeVersenken_Projekt_ Battleship.cpp
        Board.cpp
        Board.h
        Opponent.cpp
        Opponent.h
        Persistance.cpp
        Persistance.h
        GameLoop.cpp
        GameLoop.h
        CppRandom.hpp)
