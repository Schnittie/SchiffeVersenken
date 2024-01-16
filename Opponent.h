//
// Created by laure on 22.12.2023.
//

#ifndef SCHIFFEVERSENKEN_PROJEKT__OPPONENT_H
#define SCHIFFEVERSENKEN_PROJEKT__OPPONENT_H


#include <memory>
#include "Board.h"
#include "Coordinates.h"

class Coordinates;
class Opponent {
public:
    // fügt ein neues Schiff in das Board ein und gibt das Board zurück
    static std::unique_ptr<Board> addRandomShipOfGivenSize(std::unique_ptr<Board>, int);
    static std::unique_ptr<Board> placeAllShips(std::unique_ptr<Board>);
    static std::unique_ptr<Board> makeGuess(std::unique_ptr<Board>, int);
    static std::unique_ptr<Board> guessRandom(std::unique_ptr<Board>, std::vector<Coordinates>);

private:
    static int getNumberOfDiscoveredFieldsOfShip(std::unique_ptr<Board>, Coordinates);
    static int getNumberOfDiscoveredFieldsOfShipInThisDirection(std::unique_ptr<Board>, Coordinates, Direction);

//TODO: Was ist ein Opponent?
// Meine Überlegung (Fleischi): Opponent = das was ein möglicher realer Gegenspieler tun würde
// -> ist "dumm" und bekommt bei jeder Methode das Board mit rein
// -> Basically braucht man nur 2 Methoden, eine die ein Schiff platziert und eine die einen "Schuss" abgibt
//TODO: Idee wir könnten wenn wir fancy sein wollen auch mehrere Opponents machen mit verschiedenen schwierigkeitsgraden welche dann alle ein Interface erfüllen
// Meine Überlegung (Fleischi): deutlich einfacher einen Score zu machen, der dann in platzieren Methoden einbezogen wird, als mehrere Klassen

};


#endif //SCHIFFEVERSENKEN_PROJEKT__OPPONENT_H
