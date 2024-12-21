#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "src/board.h"
#include "src/save.h"

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
            case 'm':{
                sscanf(optarg, "%zux%zux%zu", &rows, &cols, &bombs);
                printf("Making game with size %zux%zu and %zu bombs.\n", rows, cols, bombs);
                board* Board2 = make_board(rows, cols, bombs);
                board_assert(Board2);

                print_board_game(Board2);

                size_t row, col;
                printf("Enter row and column[row column]: ");

                assert(scanf("%zu %zu", &row, &col) == 2);
                assert(row >= 0 && row < Board2->rows && col >= 0 && col < Board2->cols);
                randomize_board(Board2, row, col);
                switch (Board2->P[row][col]) {
                    case -1:
                        Board2->P[row][col] = 0;
                        break;
                }
                print_board_game(Board2);
                // główna pętla gry
                // na koniec zwolnienie pamięci 
                free(Board2);
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
