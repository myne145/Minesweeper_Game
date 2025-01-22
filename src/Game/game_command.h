//
// Created by antekk on 12/29/24.
//

#ifndef MINESWEEPER_JIMP1_GAME_COMMAND_H
#define MINESWEEPER_JIMP1_GAME_COMMAND_H

/**
 * Pokazuje zapytanie tak czy nie
 * @param text tekst
 * @param defaultYes czy domyślną opcją jest TAK
 * @return czy user dał tak czy nie
 */
int show_yes_no_input_field(char* text, int defaultYes);


/**
 * Dzieli komendę po spacjach na tablicę stringów
 * @param line string z komendą
 * @param commandLength adres wartości gdzie przetrzymujemy długość tablicy z komendą
 * @return tablica z komendą
 */
char** split_command_by_spaces(char* line, int* commandLength);

/**
 * Zwalnia pamięć zajmowaną przez komendę (char**)
 * @param command tablica z komendą
 * @param commandLength długość tej tablicy
 */
void free_command(char** command, int commandLength);

#endif //MINESWEEPER_JIMP1_GAME_COMMAND_H
