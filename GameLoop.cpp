//
// Created by laure on 22.12.2023.
//

#include <iostream>
#include <thread>
#include "GameLoop.h"
#include "Opponent.h"
#include "Persistance.h"
#include "HelpFunctions.h"

void GameLoop::startGame(std::unique_ptr<Board> playerBoard, std::unique_ptr<Board> opponentBoard, int difficulty) {
    bool quitRound = false;
    if (!HelpFunctions::valuesOfShipsLeftToSetAreZero(std::move(opponentBoard->createCopy()))) {
        opponentBoard = Opponent::placeAllShips(std::move(opponentBoard));
        std::cout << std::endl << "Der Gegner hat alle seine Schiffe gesetzt.";
    }
    // der Gegner setzt den Rest seiner Schiffe, falls sie noch nicht gesetzt sind
    std::cout << std::endl << std::endl << "Ihr Feld:";
    playerBoard->printShipField();
    // das Feld des Spielers wird zu Orientierung ausgegeben
    std::vector<std::string> inputsVector;
    playerBoard = tryToRequestAllShipsSet(std::move(playerBoard));
    // man versucht alle Schiffe vom Spieler gesetzt zu bekommen
    if (HelpFunctions::valuesOfShipsLeftToSetAreZero(std::move(playerBoard->createCopy()))) {
        std::cout << std::endl << "Nun geht es ans Felder aufdecken!" << std::endl;
    } else {
        quitRound = true;
    }
    // wenn nicht alle Schiffe gesetzt wurden, heißt das, dass der Spieler zu Hauptmenü möchte
    while (playerBoard->totalShipsNotSunk > 0 && opponentBoard->totalShipsNotSunk > 0 && !quitRound) {
        // wiederhole so lange, bis alle Schiffe eines zerstört wurden, oder der Spieler die Runde verlassen möchte
        int numberOfGuessesBefore;
        do {
            numberOfGuessesBefore = opponentBoard->guessCounter;
            // speichere die Nummer der Guesses der Spieler zwischen um nach Rückerhalt des Boards
            // nachvollziehen zu können, ob der Spieler erfolgreich getippt hat und um insgesamt die Anzahl der guesses
            // speichern zu können
            requestNewGuess(std::move(opponentBoard->createCopy()));
            // fordere einen neuen Rate-Versuch des Spielers an
            do {
                inputsVector = HelpFunctions::readCInIntoVector();
            } while (inputsVector.empty());
            // warte, bis der Spieler eine Eingabe gemacht und nicht nur eine neue Zeile angefangen hat
            if (HelpFunctions::vectorHasOneValueWithOneChar(inputsVector) && inputsVector.at(0).at(0) == '0') {
                printInstructions();
            } else if (HelpFunctions::vectorHasOneValueWithOneChar(inputsVector) && inputsVector.at(0).at(0) == '1') {
                std::cout << "Das ist die Platzierung Ihrer Schiffe:";
                playerBoard->printShipField();
            } else if (HelpFunctions::vectorHasOneValueWithOneChar(inputsVector) && inputsVector.at(0).at(0) == '2') {
                quitRound = true;
            } else if (HelpFunctions::vectorHasOneValueWithOneChar(inputsVector) && inputsVector.at(0).at(0) == '3') {
                Persistance::saveGame({(std::move(playerBoard->createCopy())),
                                       std::move(opponentBoard->createCopy()), 7});
            } else {
                opponentBoard = interpretGuess(std::move(opponentBoard), inputsVector);
            }
            // wenn nur ein Zeichen eingegeben wurde und dieses einer Aktion zugewiesen ist, führe diese Aktion durch,
            // ansonsten versuche die Eingabe als Rate-Versuch zu interpretieren
        } while (opponentBoard->guessCounter == numberOfGuessesBefore && !quitRound);
        // wiederhole so lange, bis ein Rate-Versuch des Spielers gültig war oder der Spieler das Spiel verlassen möchte
        std::this_thread::sleep_for(std::chrono::seconds(3));
        // halte den Thread kurz an, damit der Spieler erkennen kann, ob sein Rate-Versuch erfolgreich war
        if (opponentBoard->totalShipsNotSunk > 0 && !quitRound) {
            playerBoard = letOpponentGuess(std::move(playerBoard), difficulty);
            playerBoard->guessCounter++;
        }
        // wenn der Spieler noch nicht alle Schiffe des Gegners zerstört hat und nicht die Runde verlassen möchte,
        // macht der Opponent einen Rate-Versuch
    }
    if (playerBoard->totalShipsNotSunk <= 0) {
        std::cout << std::endl << "Der Gegner hat alle Ihre Schiffe zerstoert und dadurch gewonnen!!!" << std::endl <<
            "Er hat dafuer " << playerBoard->guessCounter << " mal tippen muessen!" << std::endl << std::endl<<
            "Das gegnerische Schlachtfeld waere gewesen:";
            opponentBoard->printShipField();
    } else if (opponentBoard->totalShipsNotSunk <= 0) {
        std::cout << std::endl << "Sie haben alle Schiffe des Gegners zerstoert und dadurch gewonnen!!! Glueckwunsch!!!" << std::endl <<
                               "Sie haben dafuer " << opponentBoard->guessCounter << " mal tippen müssen!" << std::endl;
    }
    // wenn die Schleife für die Rate-Versuche verlassen wurde und dabei ein Spieler alle Schiffe des Gegners zerstört
    // hat, wird eine Nachricht für das Gewinnen/Verlieren ausgegeben
}

void GameLoop::requestNewShipField(std::unique_ptr<Board> playerBoard) {
    std::cout << "Sie setzen als naechstes ein Schiff der Groesse " <<
    getSizeOfBiggestShipLeftToSet(std::move(playerBoard->createCopy())) << ".";
    std::cout << std::endl << "Bitte geben Sie die Werte des Ausgangsfelds durch ein Leerzeichen getrennt an!";
    std::cout << std::endl << "( (0) Anweisungen und Regeln    (1) Spiel abbrechen und zurueck zum Hauptmenue";
    std::cout << std::endl << "  (2) restliche Schiffe zufaellig setzen )" << std::endl;
}

int GameLoop::getSizeOfBiggestShipLeftToSet(std::unique_ptr<Board> board) {
    int sizeOfBiggestShipLeftToSet = 0;
    for (int shipSize = 5; shipSize >= 2; shipSize--) {
        if (board->shipsLeftToSet.find(shipSize)->second > 0) {
            sizeOfBiggestShipLeftToSet = shipSize;
            break;
        }
    }
    return sizeOfBiggestShipLeftToSet;
    // gehe die Werte von shipsLeftToSet anfangend vom größten durch und gebe die Schiffsgröße zurück,
    // bei der zuerst noch ein Schiff zu setze ist
}

std::unique_ptr<Board> GameLoop::tryToRequestAllShipsSet(std::unique_ptr<Board> board) {
    bool quitRound = false;
    std::vector<std::string> inputsVector;
    while (!HelpFunctions::valuesOfShipsLeftToSetAreZero(std::move(board->createCopy())) && !quitRound) {
        // wiederhole so lange, bis entweder von allen Schiffsgrößen in ShipsLeftToSet im Board keine mehr gesetzt werden
        // müssen oder quitRound auf true gesetzt wurde
        requestNewShipField(std::move(board->createCopy()));
        // fordert den Spieler an, "Koordinaten" einzugeben, für das Feld, von dem ausgehend er das Schiff setzen möchte
        do {
            inputsVector = HelpFunctions::readCInIntoVector();
        } while (inputsVector.empty());
        // warte, bis der Spieler eine Eingabe gemacht und nicht nur eine neue Zeile angefangen hat
        if (HelpFunctions::vectorHasOneValueWithOneChar(inputsVector)) {
            if (inputsVector.at(0).at(0) == '0') {
                printInstructions();
                inputsVector.clear();
            } else if (inputsVector.at(0).at(0) == '1') {
                quitRound = true;
            } else if (inputsVector.at(0).at(0) == '2') {
                board = Opponent::placeAllShips(std::move(board));
            } else {
                invalidInput();
            }
            // wenn nur ein Zeichen eingegeben wurde und dieses entweder 1, 2 oder 3 ist, dann führe die entsprechende Funktion aus,
            // wenn dieses Zeichen nichts davon ist, dann ist dies eine invalide Eingabe
        } else {
            Coordinates coordinates = turnStringVectorIntoCoordinates(inputsVector);
            if (inputsVector.size() != 2 || !GameRule::insideField(coordinates, board->size)) {
                inputsVector.clear();
                // wenn die Anzahl der Inputs nicht 2 ist oder die Koordinaten, die auf Basis des Inputs generiert wurden
                // nicht im Feld liegen, dann ist dies eine invalide Eingabe
            } else {
                board = requestShipDirectionAndTrySet(std::move(board), coordinates,
                                                      getSizeOfBiggestShipLeftToSet(std::move(board->createCopy())));
            }
            // wenn nicht, dann führe requestShipDirectionAndTrySet aus, versuche also mit den gegebenen Eingaben nach dem Erfragen
            // der Richtung, in die das Schiff gerichtet sein soll ein Schiff zu setzen
        }
    }
    return std::move(board);
}

std::unique_ptr<Board> GameLoop::requestShipDirectionAndTrySet(std::unique_ptr<Board> playerBoard, Coordinates coordinates,
                                                               int sizeOfBiggestShipLeftToSet) {
    std::cout << "Bitte geben Sie die Richtung, in die das Schiff gerichtet sein soll, an! ('o'^|'u'v|'l'<|'r'>)" << std::endl;
    std::string input;
    do {
        std::cin >> input;
    } while (input.empty());
    // warte, bis der Spieler eine Eingabe gemacht und nicht nur eine neue Zeile angefangen hat
    if (input.length() != 1 || !(input.at(0) == 'l' || input.at(0) == 'r' || input.at(0) == 'o'
                            || input.at(0) == 'u' || input.at(0) == '0')) {
        invalidInput();
        return std::move(playerBoard);
    }
    // wenn entweder nicht genau ein Zeichen eingegeben wurde oder die Eingabe nicht eines der erwarteten Zeichen ist,
    // ist das ein invalide Eingabe
    if (input.at(0) == '0') {
        printInstructions();
        return std::move(playerBoard);
    } // wenn die Eingabe '0' ist, dann gebe die Anweisungen und Regeln aus und beende den Vorgang
    if (!playerBoard->addShip(sizeOfBiggestShipLeftToSet, coordinates, Coordinates::charIntoDirection(input.at(0)))) {
        invalidInput();
        return std::move(playerBoard);
    } // wenn das Hinzufügen des Schiffes nicht erfolgreich war, dann war das ein invalide Eingabe
    playerBoard->printShipField();
    return std::move(playerBoard);
    // wenn alles korrekt war, gebe das Board mit dem neuen Schiff aus und beende die Funktion
    // (Das Schiff wurde durch playerBoard->addShip(...) hinzugefügt, da dieses ja nicht fehlgeschlagen ist)
}

void GameLoop::requestNewGuess(std::unique_ptr<Board> opponentBoard) {
    std::cout << "Ihr bisheriges Resultat:";
    opponentBoard->printGuessField();
    std::cout << "Sie duerfen auf ein Feld tippen.";
    std::cout << std::endl << "Bitte geben Sie die Werte des Felds durch ein Leerzeichen getrennt an!";
    std::cout << std::endl << "( (0) Anweisungen und Regeln   (1) Mein Schiffsfeld";
    std::cout << std::endl << "  (2) Spiel abbrechen und zurueck zum Hauptmenue   (3) Spiel speichern )" << std::endl;
}

std::unique_ptr<Board> GameLoop::interpretGuess(std::unique_ptr<Board> opponentBoard, const std::vector<std::string>& inputsVector) {
    // der inputs vector enthält die einzelnen Eingaben des Spielers an den Leerzeichen getrennt
    Coordinates coordinates = turnStringVectorIntoCoordinates(inputsVector);
    if (inputsVector.size() != 2 || !GameRule::insideField(coordinates, opponentBoard->size)) {
        invalidInput();
        return std::move(opponentBoard);
        // wenn entweder nicht genau zwei Zeichenketten eingegeben wurden oder sich die Koordinaten, die auf Basis der
        // Zeichenketten erstellt wurden, nicht im Spielfeld befinden, dann war das ein invalide Eingabe
    }
    GuessStatus guessResult = opponentBoard->makeGuess(coordinates);
    // versuche den Guess auf das Board anzuwenden
    switch (guessResult) {
        case GuessStatus::sunkShip:
            std::cout << std::endl << "Sie haben ein Schiff versenkt!" << std::endl;
            break;
        case GuessStatus::guessedRight:
            std::cout << std::endl << "Sie haben ein Schiff getroffen!" << std::endl;
            break;
        case GuessStatus::guessedWrong:
            std::cout << std::endl << "Sie haben nichts getroffen!" << std::endl;
            break;
        default:
            invalidInput();
            return std::move(opponentBoard);
    }
    // eine Nachricht wird je nach Treffer/nicht-Treffer ausgegeben, oder, falls der Guess ungültig war
    // (z.B. weil dieses Feld schon guessed wurde), dann war das ein invalide Eingabe
    opponentBoard->guessCounter++;
    // sonst war der Guess erfolgreich → der GuessCounter wird erhöht
    std::cout << std::endl << "Ihr bisheriger Fortschritt:";
    opponentBoard->printGuessField();
    std::cout << "Um zu gewinnen muessen Sie noch " << opponentBoard->totalShipsNotSunk << " Schiffe versenken!" << std::endl;
    // dem Spieler werden Informationen über seinen Spielfortschritt ausgegeben
    return std::move(opponentBoard);
}

std::unique_ptr<Board> GameLoop::letOpponentGuess(std::unique_ptr<Board> playerBoard, int smartness) {
    std::cout << std::endl << "Der Gegner ist dran:";
    playerBoard = Opponent::makeGuess(std::move(playerBoard), smartness);
    playerBoard->printGuessField();
    std::cout << "Der Gegner muss noch " << playerBoard->totalShipsNotSunk << " Schiffe versenken, um zu gewinnen." << std::endl;
    return std::move(playerBoard);
    // der Gegner macht einen Guess und das Resultat dessen wird dem Spieler angezeigt
}

void GameLoop::printInstructions() {
    std::cout << std::endl << "SPIELANWEISUNGEN: ";
    std::cout << std::endl << "Die Position von Feldern muss immer durch Eingabe des sie bezeichnenden Buchstaben "
                              "und der Zahl durch ein Leerzeichen " << std::endl <<
                              "getrennt angegeben werden.";
    std::cout << std::endl << "Beim Setzen von Schiffen wird das Ausgangsfeld des Schiffs und die Richtung, "
                              "in die es gerichtet ist nacheinander " << std::endl <<
                              "erwartet. Das Ausgangsfeld wird wie im vorherigen "
                              "Absatz beschrieben angegeben. Die Richtung wird als Anfangsbuchstaben" << std::endl <<
                              "der folgenden Auswahl an Richtungen erwartet: ";
    std::cout << std::endl << "1.links(-> 'l') 2.rechts(-> 'r') 3.oben(-> 'o') 4.unten(-> 'u')";
    std::cout << std::endl << "Beispiel: Wenn ein Schiff der Groesse 3 angegeben werden soll, als erste Eingabe '7 D' und "
                              "als zweite Eingabe 'u' " << std::endl <<
                              "erfolgt, bedeutet das, dass sich das Schiff ueber die Felder 7-D, 7-E und 7-F erstreckt";
    std::cout << std::endl << "Beim Tippen eines Feldes reicht lediglich die wie zuvor beschriebene Angabe eines Feldes.";
    std::cout << std::endl << std::endl << "SPIELREGELN:";
    std::cout << std::endl << "Das Feld besteht anfangs nur aus Wasser (Markierung: ~).";
    std::cout << std::endl << "Zunaechst muessen alle Schiffe gesetzt werden (das Programm fordert die jeweils zu setzende "
                              "Anzahl an Schiffen in der " << std::endl <<
                              "jeweiligen Groesse an), diese duerfen nicht um die Ecke gehen, "
                              "muessen komplett innerhalb des Spielfelds liegen und " << std::endl <<
                              "duerfen ein nebenliegendes Schiff nicht "
                              "beruehren. (Die auf dem eigenen Feld gesetzten Schiffe werden mit dem Zeichen @" << std::endl <<
                              "dargestellt.) Ist das erledigt, kann ab sofort nach jedem Spielzug gespeichert werden.";
    std::cout << std::endl << "Dann darf der Spieler anfangen auf eine Position des Feldes, auf dem der Gegner seine "
                              "Schiffe platziert hat, zu tippen. " << std::endl <<
                              "Dieses wird daraufhin aufgedeckt und markiert (Markierung: x), "
                              "falls sich ein Teil eines Schiffes darunter befindet. " << std::endl <<
                              "Auf jedes Feld kann nur einmal getippt werden. Wenn alle Positionen, "
                              "an denen sich das Schiff befindet dadurch " << std::endl <<
                              "markiert sind, ist dieses versenkt (Markierung: #). Wer zuerst alle Schiffe des "
                              "Gegners versenkt hat, gewinnt das Spiel!" << std::endl << std::endl;
}

void GameLoop::invalidInput() {
    std::cout << std::endl << "Dies ist eine ungueltige Eingabe!" << std::endl;
}

Coordinates GameLoop::turnStringVectorIntoCoordinates(std::vector<std::string> inputsVector) {
    if (inputsVector.size() != 2) {
        invalidInput();
        return {-1, -1};
        // Es wird ein Vektor mit zwei Werten erwartet → alles andere ist eine ungültige Eingabe → Rückgabe ungültiger Koordinaten
    }
    int xPos = 0;
    int yPos;
    if (inputsVector.at(0).size() == 1 && inputsVector.at(0).at(0) >= 65) {
        yPos = static_cast<int>(inputsVector.at(0).at(0)) - 65;
        for (char digit: inputsVector.at(1)) {
            xPos = xPos * 10 + (static_cast<int>(digit) - 48);
        }
        xPos--;
    } else if (inputsVector.at(1).size() == 1 && inputsVector.at(1).at(0) >= 65) {
        yPos = static_cast<int>(inputsVector.at(1).at(0)) - 65;
        for (char digit: inputsVector.at(0)) {
            xPos = xPos * 10 + (static_cast<int>(digit) - 48);
        }
        xPos--;
    } else {
        invalidInput();
        return {-1, -1};
    }
    // einer der Werte muss ein einzelnes Zeichen sein mit einem Ascii-Code >= 65 (es handelt sich um die y-Koordinate,
    // die als Buchstabe erwartet wird), wenn dies nicht so ist, dann ist das eine invalide Eingabe
    // der zuvor benannte Wert wird zu einer Zahl umgewandelt und -65 gerechnet, um einen Wert von 0-30 zu bekommen
    // (A=0, B=1, ...) und wird dann als yPos interpretiert, beim anderen wird der Zwischenwert von xPos so lage *10
    // gerechnet und die nächste Ziffer in eine int gecastet - 48 gerechnet und aufaddiert, bis alle Ziffern durch sind
    // (Ascii-Code von 1=48, 2=49, ...) → der Wert wird vom String in eine Int umgewandelt
    return {xPos, yPos};
}

void GameLoop::printMainMenu() {
    std::cout << std::endl << "HAUPTMENUE:" << std::endl;
    std::cout << std::endl << "Ihre Optionen:" << std::endl <<
              "  (0) Anweisungen und Regeln       (1) neues Spiel starten" << std::endl <<
              "  (2) gespeichertes Spiel laden    (3) Spielstand loeschen" << std::endl <<
              "  (4) beenden" << std::endl;
}

void GameLoop::startNewGame() {
    std::string input;
    std::cout << std::endl << "Welche Groesse soll das Spielfeld haben? (Erlaubte Groessen:5-30 Normal:10)" << std::endl;
    int sizeInput = 0;
    do {
        if (sizeInput != 0) {
            invalidInput();
            std::cout << "Bitte versuchen Sie es erneut:" << std::endl;
            // Eingabe hat nicht den erwarteten Werten genügt → Fehlermeldung
        }
        std::cin >> input;
        sizeInput = HelpFunctions::stringToInt(input);
        // wandelt den String in einen int um
    } while (!(sizeInput <= 30 && sizeInput >= 5));
    // wiederhole so lange, bis eine valide Eingabe erfolgt
    std::cout << std::endl << "Welchen Schwierigkeitsgrad moechten Sie waehlen? (Schwierigkeitsgrade:1-10 10=schwerstes)" << std::endl;
    int difficultyInput = 0;
    do {
        if (difficultyInput != 0) {
            invalidInput();
            std::cout << "Bitte versuchen Sie es erneut:" << std::endl;
            // Eingabe hat nicht den erwarteten Werten genügt → Fehlermeldung
        }
        std::cin >> input;
        difficultyInput = HelpFunctions::stringToInt(input);
        // wandelt den String in einen int um
    } while (!(difficultyInput <= 10 && difficultyInput >= 1));
    // wiederhole so lange, bis eine valide Eingabe erfolgt
    std::unique_ptr<Board> playerBoard = std::make_unique<Board>(sizeInput);
    std::unique_ptr<Board> opponentBoard = std::make_unique<Board>(sizeInput);
    std::cout << std::endl << "DAS SPIEL BEGINNT" << std::endl;
    GameLoop::startGame(std::move(playerBoard), std::move(opponentBoard), difficultyInput);
    // erstellt die entsprechenden Boards und startet das Spiel
}

void GameLoop::tryLoadGame() {
    GameState gameState = Persistance::loadGame();
    if (gameState.playerBoard != nullptr && gameState.opponentBoard != nullptr) {
        gameState.playerBoard->allShipsAlreadySet();
        gameState.opponentBoard->allShipsAlreadySet();
        GameLoop::startGame(std::move(gameState.playerBoard), std::move(gameState.opponentBoard), gameState.opponentLevel);
        // wenn das Laden des Spiels erfolgreich war und beide Boards im gameState Objekt vorhanden sind,
        // wird das Spiel mit dem geladenen Spielstand gestartet
    } else {
        std::cout << std::endl << "Spielstand konnte nicht geladen werden" << std::endl;
    }
}

