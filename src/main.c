#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "Board/board.h"
#include "Game/game.h"
#include "GameStats/game_stats.h"
#include "SaveAndLoadGame/save_load.h"
#include "RayLibGUI/ray.h"

void set_values_from_preset(int preset, size_t* rows, size_t* cols, size_t* bombs)
{
    if (preset == 1)
    {
        *rows = 9;
        *cols = 9;
        *bombs = 10;
    }
    else if (preset == 2)
    {
        *rows = 16;
        *cols = 16;
        *bombs = 40;
    } else if(preset == 3)
    {
        *rows = 16;
        *cols = 30;
        *bombs = 99;
    }
}

void show_best_players() {
    int amountOfPlayersToShow;
    printf("Enter n:\t");
    scanf("%d", &amountOfPlayersToShow);
    assert(amountOfPlayersToShow > 0);
    player** players = load_n_best_players_from_stats_file(&amountOfPlayersToShow);
    printf("%d best players:\n", amountOfPlayersToShow);
    print_players(players, amountOfPlayersToShow);
    for(int i = 0; i < amountOfPlayersToShow; i++)
        free_player(players[i]);
    free(players);
}


int main(int argc, char** argv) {
    int launchOption = getopt(argc, argv, ":l:g");

    if(launchOption == 'g') {
        gui();
        exit(EXIT_SUCCESS);
    }

    size_t rows, cols, bombs;
    size_t seed = time(NULL);
//    size_t seed = 1737580773;
    srand(seed);
    board* gameBoard;


    //ładowanie gry z pliku
    if(launchOption == 'l') {
        gameBoard = load_game(optarg);
        printf("-=-=-=-=-=-=-=-=-=Loading a game from file-=-=-=-=-=-=-=-=-=-\n");
        printf("Filename: %s\n", optarg);
        printf("Game score: %f\n\n", gameBoard->score);
        print_board_stats(seed, gameBoard);
        printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n");
        start_game_from_saved_board(gameBoard);
        exit(EXIT_SUCCESS);
    }


    printf("-=-=-=-=-=-=-=-=-=-Select game difficulty-=-=-=-=-=-=-=-=-=-\n");
    printf("[1] Easy - 9x9 fields, 10 mines\n");
    printf("[2] Medium - 16x16 fields, 40 mines\n");
    printf("[3] Hard - 16x30 fields, 99 mines\n");
    printf("[4] Custom\n");
    printf("or [5] to print n best players\n");

    int selectedOption = fgetc(stdin) - '0';
    if(fgetc(stdin) != '\n') { //usuwamy \n z stdin i sprawdzamy czy user nie podał więcej niz 1 symbola
        fprintf(stderr, "Invalid option\n");
        exit(EXIT_FAILURE);
    }

    //drukowanie najlepszych graczy
    if(selectedOption == 5) {
        show_best_players();
        exit(EXIT_SUCCESS);
    }


    printf("-=-=-=-=-=-=-=-=-=-=-=-Creating a game-=-=-=-=-=-=-=-=-=-=-=-\n");

    if(selectedOption >= 1 && selectedOption <= 3) {
        //gra z presetu
        set_values_from_preset(selectedOption, &rows, &cols, &bombs);
    } else {
        //gra niestandardowa
        printf("Enter desired board settings [rows cols mines]:\t");
        scanf("%zu %zu %zu", &rows, &cols, &bombs);
    }
    gameBoard = make_board(rows, cols, bombs);
    board_assert(gameBoard);
    print_board_stats(seed, gameBoard);
    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n");
    start_game_from_board(gameBoard);

    return 0;
}
