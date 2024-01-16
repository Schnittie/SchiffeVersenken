#include <iostream>
#include "Board.h"
#include "Opponent.h"
#include "Persistance.h"
#include <memory>


int main() {
    std::unique_ptr<Board> board = std::make_unique<Board>(10);
    std::unique_ptr<Opponent> opponent = std::make_unique<Opponent>();
    board = opponent->placeAllShips(std::move(board));
    // wichtig! logisch: nicht mehr Guesses als size^2 möglich
    for (int i = 0; i < 90; i++) {
        board = opponent->makeGuess(std::move(board));
    }
    Persistance::saveGame({(std::move(board->createCopy())), std::move(board->createCopy()), 7});
    board->printShipField();
    board->printGuessField();
    GameState gameState = Persistance::loadGame();
    gameState.playerBoard->printShipField();
    gameState.playerBoard->printGuessField();
    return 0;
}

