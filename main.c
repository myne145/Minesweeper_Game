#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
                board* loadedBoard = load_game(optarg);
                print_board_game(loadedBoard);
                start_game_from_board(loadedBoard);
                break;
            }
            case 'm':{
                sscanf(optarg, "%zux%zux%zu", &rows, &cols, &bombs);
                size_t seed = time(NULL);
                // size_t seed = 1735042975;
                printf("Making game with size %zux%zu and %zu bombs.\nSeed: %zu\n", rows, cols, bombs, seed);
                srand(seed);
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
