#include <iostream>
#include "Board.h"
#include "Opponent.h"
#include <memory>


int main() {
    std::unique_ptr<Board> board = std::make_unique<Board>();
    std::unique_ptr<Opponent> opponent = std::make_unique<Opponent>();
    board = opponent->placeAllShips(std::move(board));
    board->printShipField();
    return 0;
}

