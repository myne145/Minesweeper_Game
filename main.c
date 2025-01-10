#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "src/Board/board.h"
#include "src/SaveAndLoadGame/save_load.h"
#include "src/Game/game.h"

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
    while((option = getopt(argc, argv, "l:m:hp:")) != -1) {
        size_t rows, cols, bombs;
        size_t seed = time(NULL);
        srand(seed);

        board* gameBoard;
        switch (option) {
            case 'l':
                gameBoard = load_game(optarg);

                printf("-=-=-=-=-=-=-=-=-=Loading a game from file-=-=-=-=-=-=-=-=-=-\n");
                printf("Filename: %s\n", optarg);
                printf("Game score: %f\n\n", gameBoard->score);
                print_board_stats(seed, NULL, gameBoard);
                printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n");

                start_game_from_saved_board(gameBoard);
                break;

            case 'm':
                sscanf(optarg, "%zux%zux%zu", &rows, &cols, &bombs);
                gameBoard = make_board(rows, cols, bombs);

                printf("-=-=-=-=-=-=-=-=-=-=-=-Creating a game-=-=-=-=-=-=-=-=-=-=-=-\n");
                print_board_stats(seed, NULL, gameBoard);
                printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n");

                start_game_from_board(gameBoard);
                break;

            case 'p':
                char* preset = malloc(6 * sizeof(char)); //easy, medium, hard
                sscanf(optarg, "%s", preset);
                set_values_from_preset(preset, &rows, &cols, &bombs);
                gameBoard = make_board(rows, cols, bombs);

                printf("-=-=-=-=-=-=-=-=-=-=-=-Creating a game-=-=-=-=-=-=-=-=-=-=-=-\n");
                print_board_stats(seed, preset, gameBoard);
                printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n");

                free(preset);
                start_game_from_board(gameBoard);
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
