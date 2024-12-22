//
// Created by antekk on 12/22/24.
//
#ifndef GAME_H
#define GAME_H
#include "board.h"

/**
 * Zaczyna grę z podanej planszy
 * @param startingBoard plansza z której chcemy rozpocząć grę
 */
void start_game_from_board(board* startingBoard);

/**
 * W otoczeniu pola (row,column) szuka i odsłania wszystkie pola które są puste (nie są bombami, nie sąsiadują z żadnymi)
 * @param gameBoard plansza
 * @param row rząd
 * @param col kolumna
 */
void show_surrounding_empty_fields(board* gameBoard, int row, int col);

#endif //GAME_H
