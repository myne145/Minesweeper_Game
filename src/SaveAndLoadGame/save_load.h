#ifndef SAVE_H
#define SAVE_H

#include "../Board/board.h"

/**
 * Funkcja zapisująca grę do pliku binarnego wybranego przez gracza
 * @param save_name - nazwa pliku do zapisu
 * @param gameBoard - wskaźnik na planszę
 */
static void save_game(char* save_name, board* gameBoard);

/**
 * Funkcja wczytująca grę z pliku binarnego wybranego przez gracza
 * @param save_name - nazwa pliku do odczytu
 * @return plansza którą wczytaliśmy
 */
board* load_game(char* save_name);

/**
 * Zapisuje grę do podanego
 * @param gameBoard
 * @param filename
 */
void save_with_exit_confirmation(board* gameBoard, char* filename);

#endif // SAVE_H
