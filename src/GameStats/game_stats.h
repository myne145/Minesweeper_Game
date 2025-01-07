//
// Created by antekk on 1/1/25.
//

#ifndef MINESWEEPER_JIMP1_GAME_STATS_H
#define MINESWEEPER_JIMP1_GAME_STATS_H

#include <stdlib.h>
#include "../Board/board.h"

/**
 * Struktura przechowujaca gracza
 */
typedef struct player {
    char* name;
    double score;
    struct timeval* playerGameTime;
} player;

/**
 * Funkcja inicjalizująca strukturę player
 * @param name
 * @param score
 * @param gameTime
 * @return zainicjalizowana struktura
 */
player* create_player(char* name, double score, struct timeval* gameTime);

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

/**
 * Zwalnia pamiec zajmowana przez gracza
 * @param player1 gracz
 */
void free_player(player* player1);


/**
 * wylicza czas gry używajac lokalnego czasu
 * @param gameBoard plansza ktorej czas chcemy wyliczyc
 */
void calculate_game_board_time_using_local_time(board* gameBoard);


/**
 * Drukuje tablice graczy
 * @param players tablica graczy
 * @param length długość tablicy graczy
 */
void print_players_(player** players, int length);


#endif //MINESWEEPER_JIMP1_GAME_STATS_H
