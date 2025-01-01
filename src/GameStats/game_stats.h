//
// Created by antekk on 1/1/25.
//

#ifndef MINESWEEPER_JIMP1_GAME_STATS_H
#define MINESWEEPER_JIMP1_GAME_STATS_H

#include <stdlib.h>

//struktura przechowująca gracza
typedef struct player {
    char* name;
    double score;
    size_t timeInMillis;
} player;

/**
 * Funkcja inicjalizująca strukturę player
 * @param name
 * @param score
 * @param timeInMillis
 * @return zainicjalizowana struktura
 */
player* create_player(char* name, double score, size_t timeInMillis);

/**
 * Dodaje gracza do pliku ze statystykami
 * @param newPlayer
 */
void add_player_to_stats_file(player* newPlayer);

/**
 * Próbuje wczytać n graczy z pliku ze stattystykami
 * @param n ilość graczy jaką chcemy wczytać i bufor który po wykonaniu funkcji zostanie ustawiony na faktyczną ilość graczy w tablicy
 * @return tablica struktur z graczami
 */
player** load_n_best_players_from_stats_file(int* n);

#endif //MINESWEEPER_JIMP1_GAME_STATS_H
