#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "src/board.h"
#include "src/save.h"
#include "src/game.h"

void set_values_from_preset(char* preset, size_t* rows, size_t* cols, size_t* bombs)
{
    if (strcmp(preset, "easy") == 0)
    {
        *rows = 9;
        *cols = 9;
        *bombs = 10;
    }
    else if (strcmp(preset, "medium") == 0)
    {
        *rows = 16;
        *cols = 16;
        *bombs = 40;
    }
    else if (strcmp(preset, "hard") == 0)
    {
        *rows = 16;
        *cols = 30;
        *bombs = 99;
    }
    else
    {
        fprintf(stderr, "Invalid preset\n");
        exit(0);
    }

}

int main(int argc, char** argv) {
    int option;
    while((option = getopt(argc, argv, "l:m:h:p:")) != -1) {
        size_t rows = 0;
        size_t cols = 0;
        size_t bombs = 0;
        size_t seed = time(NULL);
        switch (option) {
            case 'l':
                printf("Loading game from file: %s\n", optarg);
                board* loadedBoard = load_game(optarg);
                print_board_game(loadedBoard);
                start_game_from_saved_board(loadedBoard);
                break;

            case 'm':
                sscanf(optarg, "%zux%zux%zu", &rows, &cols, &bombs);

                // size_t seed = 1735042975;
                printf("Making game with size %zux%zu and %zu bombs.\nSeed: %zu\n", rows, cols, bombs, seed);
                srand(seed);
                board* gameBoard = make_board(rows, cols, bombs);
                start_game_from_board(gameBoard);
                break;

            case 'p':
                //easy
                //medium
                //hard
                char* preset = malloc(6 * sizeof(char));
                sscanf(optarg, "%s", preset);
                set_values_from_preset(preset, &rows, &cols, &bombs);
                printf("Making game with size %zux%zu and %zu bombs.\nSeed: %zu\n", rows, cols, bombs, seed);
                printf("Preset:9  %s\n", preset);
                free(preset);
                srand(seed);
                board* gameBoard1 = make_board(rows, cols, bombs);
                start_game_from_board(gameBoard1);

                break;


            case 'h':
                printf("Help\n");
                break;

            default:
                printf("Unknown option: %c\n", optopt);
                break;
        }
    }
    return 0;
}
