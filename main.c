#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "src/board.h"
#include "src/save.h"
#include "src/game.h"



int main(int argc, char** argv) {
    int option;
    while((option = getopt(argc, argv, "l:m:h")) != -1) {
        size_t rows = 0;
        size_t cols = 0;
        size_t bombs = 0;
        switch (option) {
            case 'l':{
                printf("Loading game from file: %s\n", optarg);
                board* Board = load_game(optarg);
                board_assert(Board);
                print_board(Board);
                // główna pętla gry
                // na koniec zwolnienie pamięci
                free_board(Board);
                break;
            }
            case 'm':{ //TODO: wsparcie dla planszy wczytywanych z pliku
                sscanf(optarg, "%zux%zux%zu", &rows, &cols, &bombs);
                printf("Making game with size %zux%zu and %zu bombs.\n", rows, cols, bombs);
                board* board = make_board(rows, cols, bombs);
                start_game_from_board(board);
                break;
            }
            case 'h':{
                printf("Help\n");
                break;
            }
            default:
                printf("Unknown option: %c\n", optopt);
                break;
        }
    }
    return 0;
}
