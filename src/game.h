//
// Created by antekk on 12/22/24.
//
#ifndef GAME_H
#define GAME_H
#include "board.h"

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
 * @param gameBoard plansza
 * @param row rząd
 * @param col kolumna
 */
void show_surrounding_empty_fields(size_t row, size_t col, board* gameBoard);

#endif //GAME_H
