//
// Created by jonif on 06.01.2024.
//

#include <algorithm>
#include <iostream>
#include "GameRule.h"


bool GameRule::shipAddCorrect(int size, int xPos, int yPos, Direction direction, std::unique_ptr<Board> board) {
    // teste, ob Ursprungspunkt des Schiffs auf dem Feld
    if (size < 6 && size > 0 && xPos < 11 && xPos > 0 && yPos < 11 && yPos > 0) {
        // testen, ob weitere Schiffsfelder innerhalb von Board und nicht neben anderem Schiffsfeld
        int fieldsToSet = size;
        int recentXPos = xPos;
        int recentYPos = yPos;
        while (fieldsToSet > 0) {
            // teste, ob aktuell zu behandelndes Feld unbelegt ist
            if (recentXPos > 0 && recentYPos > 0 && recentXPos < 11 && recentYPos < 11 &&
                !board->shipField[recentXPos - 1][recentYPos - 1]) {
                // teste, ob Feld links davon unbelegt ist oder außerhalb des Feldes liegt
                bool leftFieldFree = recentXPos == 1 || !board->shipField[recentXPos - 2][recentYPos - 1];
                // teste, ob Feld rechts davon unbelegt ist oder außerhalb des Feldes liegt
                bool rightFieldFree = recentXPos == 10 || !board->shipField[recentXPos][recentYPos - 1];
                // teste, ob Feld darüber unbelegt ist oder außerhalb des Feldes liegt
                bool upperFieldFree = recentYPos == 1 || !board->shipField[recentXPos - 1][recentYPos - 2];
                // teste, ob Feld darunter unbelegt ist oder außerhalb des Feldes liegt
                bool lowerFieldFree = recentYPos == 10 || !board->shipField[recentXPos - 1][recentYPos];

                if (leftFieldFree && rightFieldFree && upperFieldFree && lowerFieldFree) {
                    // passe Variablen für (je nach Direction) neuem Feld an
                    if (direction == Direction::left) {
                        recentXPos--;
                    } else if (direction == Direction::right) {
                        recentXPos++;
                    } else if (direction == Direction::up) {
                        recentYPos--;
                    } else {
                        recentYPos++;
                    }
                    fieldsToSet--;
                } else {
                    board = nullptr;
                    return false;
                }
            } else {
                board = nullptr;
                return false;
            }
        }
        board = nullptr;
        return true;
    } else {
        board = nullptr;
        return false;
    }
}

bool GameRule::shipDestroyed(int xPos, int yPos, std::unique_ptr<Board> board) {
    if (xPos < 11 && xPos > 0 && yPos < 11 && yPos > 0 && board->shipField[xPos-1][yPos-1]) {
        // teste, ob alle Felder um Ausgangsfeld entweder außerhalb Board oder ohne Schiffsfeld sind
        if ((xPos - 1 < 1 || !board->shipField[xPos-2][yPos-1])
        && (xPos + 1 > 10 || !board->shipField[xPos][yPos-1])
        && (yPos - 1 < 1 || !board->shipField[xPos-1][yPos-2])
        && (yPos + 1 > 10 || !board->shipField[xPos-1][yPos])) {
            board = nullptr;
            return true;
        } else {
            // teste mit Untermethode, ob Schiff in alle Richtungen, in die es weiter geht, zerstört ist
            std::vector<bool> resultVector;
            if (xPos - 1 > 0 && board->shipField[xPos-2][yPos-1]) {
                resultVector.push_back(shipInThisDirectionDestroyed(xPos-1, yPos, std::move(board), Direction::left));
            }
            if (xPos + 1 < 11 && board->shipField[xPos][yPos-1]) {
                resultVector.push_back(shipInThisDirectionDestroyed(xPos+1, yPos, std::move(board), Direction::right));
            }
            if (yPos - 1 > 0 && board->shipField[xPos-1][yPos-2]) {
                resultVector.push_back(shipInThisDirectionDestroyed(xPos, yPos-1, std::move(board), Direction::up));
            }
            if (yPos + 1 < 11 && board->shipField[xPos-1][yPos]) {
                resultVector.push_back(shipInThisDirectionDestroyed(xPos, yPos+1, std::move(board), Direction::down));
            }
            return std::all_of(resultVector.begin(), resultVector.end(), [](bool resultPart) {
                return resultPart;
            });
        }
    } else {
        board = nullptr;
        return false;
    }
}

bool GameRule::shipInThisDirectionDestroyed(int xPos, int yPos, std::unique_ptr<Board> board, Direction direction) {
    int recentXPos = xPos;
    int recentYPos = yPos;
    while (true) {
        switch (direction) {
            case Direction::left:
                if (recentXPos - 1 < 1 || !board->shipField[recentXPos-2][recentYPos-1]) {
                    board = nullptr;
                    return true;
                } else if (board->guessField[recentXPos-2][recentYPos-1] == GuessStatus::guessedRight) {
                    recentXPos--;
                } else {
                    board = nullptr;
                    return false;
                }
            case Direction::right:
                if (recentXPos + 1 > 10 || !board->shipField[recentXPos][recentYPos-1]) {
                    board = nullptr;
                    return true;
                } else if (board->guessField[recentXPos][recentYPos-1] == GuessStatus::guessedRight) {
                    recentXPos++;
                } else {
                    board = nullptr;
                    return false;
                }
            case Direction::up:
                if (recentYPos - 1 < 1 || !board->shipField[recentXPos-1][recentYPos-2]) {
                    board = nullptr;
                    return true;
                } else if (board->guessField[recentXPos-1][recentYPos-2] == GuessStatus::guessedRight) {
                    recentYPos--;
                } else {
                    board = nullptr;
                    return false;
                }
            case Direction::down:
                if (recentYPos + 1 > 10 || !board->shipField[recentXPos-1][recentYPos]) {
                    board = nullptr;
                    return true;
                } else if (board->guessField[recentXPos-1][recentYPos] == GuessStatus::guessedRight) {
                    recentYPos++;
                } else {
                    board = nullptr;
                    return false;
                }
        }
    }
}
