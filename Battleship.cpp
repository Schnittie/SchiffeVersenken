#include <iostream>
#include "Board.h"
#include "Opponent.h"
#include "Persistance.h"
#include "GameLoop.h"
#include "HelpFunctions.h"
#include <memory>

int main() {
    std::cout << "    ____  ___  ______________    ___________ __  __________ \n"
                 "   / __ )/   |/_  __/_  __/ /   / ____/ ___// / / /  _/ __ \\\n"
                 "  / __  / /| | / /   / / / /   / __/  \\__ \\/ /_/ // // /_/ /\n"
                 " / /_/ / ___ |/ /   / / / /___/ /___ ___/ / __  // // ____/ \n"
                 "/_____/_/  |_/_/   /_/ /_____/_____//____/_/ /_/___/_/      \n"
                 "                                                            \n";
    bool quit = false;
    while (!quit) {
        // repeat as long as quit is not set to true
        GameLoop::printMainMenu();
        // print Main Menu with Options
        std::string input;
        do {
            input.clear();
            std::cin >> input;
        } while (input.empty());
        if(input == "0") {
            GameLoop::printInstructions();
        } else if(input == "1") {
            GameLoop::startNewGame();
        } else if(input == "2") {
            GameLoop::tryLoadGame();
        } else if(input == "3") {
            Persistance::deleteSave();
        } else if(input == "4") {
            quit = true;
        } else {
            GameLoop::invalidInput();
        }
        // react on player input
    }
    return 0;
}

