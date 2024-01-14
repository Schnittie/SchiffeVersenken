#include <iostream>
#include "Board.h"
#include "Opponent.h"
#include <memory>
#include <thread>
#include <chrono>


int main() {
    std::unique_ptr<Board> board = std::make_unique<Board>();
    std::unique_ptr<Opponent> opponent = std::make_unique<Opponent>();
    board = opponent->placeAllShips(std::move(board));
    for (int i = 0; i < 80; i++) {
        board = opponent->makeGuess(std::move(board));
    }
    board->printShipField();
    board->printGuessField();
    return 0;
}

