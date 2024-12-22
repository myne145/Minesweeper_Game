#ifndef SAVE_H
#define SAVE_H

#include "board.h"

// Funkcja zapisująca grę do pliku binarnego wybranego przez gracza
// @param save_name - nazwa pliku do zapisu
// @param Board - wskaźnik na planszę
void save_game(char* save_name, board* Board);

// Funkcja wczytująca grę z pliku binarnego wybranego przez gracza
// @param save_name - nazwa pliku do odczytu
// @param Board - wskaźnik na planszę
board* load_game(char* save_name);

#endif // SAVE_H
