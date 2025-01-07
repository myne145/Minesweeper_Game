//
// Created by antekk on 12/22/24.
//
#ifndef GAME_H
#define GAME_H
#include "../Board/board.h"

/**
 * Zaczyna grę z podanej planszy
 * @param gameBoard plansza z której chcemy rozpocząć grę
 */
void start_game_from_board(board* gameBoard);

/**
 * Zaczyna grę z zapisanej planszy (pomija pierwszy ruch i generowanie)
 * @param gameBoard plansza
 */
void start_game_from_saved_board(board* gameBoard);


/**
 * W otoczeniu pola (row,column) szuka i odsłania wszystkie pola które są puste (nie są bombami, nie sąsiadują z żadnymi)
 * @param row rząd
 * @param col kolumna
 * @param buffer bufor przechowujący ilość odkrytych pól w całym drzewie wywołań funkcji
 * @param gameBoard plansza
 */
static void show_surrounding_empty_fields(size_t row, size_t col, size_t* buffer, board* gameBoard);

#endif //GAME_H
