//
// Created by laure on 22.12.2023.
//

#ifndef SCHIFFEVERSENKEN_PROJEKT__OPPONENT_H
#define SCHIFFEVERSENKEN_PROJEKT__OPPONENT_H


#include <memory>
#include "Board.h"
#include "Coordinates.h"


class Opponent {
public:
    // "Schwierigkeitsscore" (0-10)
    // → legt fest mit welcher Wahrscheinlichkeit die Entscheidungen des computergesteuerten Gegners taktisch gefällt werden
    int smartness = 10;

    // fügt ein neues Schiff in das Board ein und gibt das Board zurück
    std::unique_ptr<Board> addRandomShipOfGivenSize(std::unique_ptr<Board>, int);
    std::unique_ptr<Board> placeAllShips(std::unique_ptr<Board>);
    std::unique_ptr<Board> makeGuess(std::unique_ptr<Board>);
    std::unique_ptr<Board> guessRandom(std::unique_ptr<Board>, std::vector<Coordinates>);

//TODO: Was ist ein Opponent?
// Meine Überlegung (Fleischi): Opponent = das was ein möglicher realer Gegenspieler tun würde
// -> ist "dumm" und bekommt bei jeder Methode das Board mit rein
// -> Basically braucht man nur 2 Methoden, eine die ein Schiff platziert und eine die einen "Schuss" abgibt
//TODO: Idee wir könnten wenn wir fancy sein wollen auch mehrere Opponents machen mit verschiedenen schwierigkeitsgraden welche dann alle ein Interface erfüllen
// Meine Überlegung (Fleischi): deutlich einfacher einen Score zu machen, der dann in platzieren Methoden einbezogen wird, als mehrere Klassen

};


#endif //SCHIFFEVERSENKEN_PROJEKT__OPPONENT_H
