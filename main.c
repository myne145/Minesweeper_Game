#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "src/Board/board.h"
#include "src/Game/game.h"
#include "src/GameStats/game_stats.h"

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

int main(int argc, char** argv) {
    int option = getopt(argc, argv, "l:m:h:p:");
    size_t rows, cols, bombs;
    size_t seed = time(NULL);
    srand(seed);
    board* gameBoard;


    if(option == 'l') {
        //level load logic
    } else if(option == 'h') { //TODO
        printf("Help:\n");
        printf("\t• -m [row]x[col]x[bombs] - creates a game with specified settings\n");
        printf("\t• -p [easy | medium | hard] - creates a game with a preset\n");
        printf("\t• -l [file] - loads a game from file\n");
        printf("\t• -s [number] - prints number best players from statistics file\n");
        printf("\t• -h - prints this message\n");
    }

    printf("-=-=-=-=-=-=-=-=-=-Select game difficulty-=-=-=-=-=-=-=-=-=-\n");
    printf("[1] Easy - 9x9 fields, 10 bombs\n");
    printf("[2] Medium - 16x16 fields, 40 bombs\n");
    printf("[3] Hard - 16x30 fields, 99 bombs\n");
    printf("[4] Custom\n");

    int c = fgetc(stdin) - '0';
    //usuwamy \n z stdin i sprawdzamy czy user nie podał więcej niz 1 symbola
    if(fgetc(stdin) != '\n') {
        printf("Invalid option\n");
        exit(EXIT_FAILURE);
    }


    printf("-=-=-=-=-=-=-=-=-=-=-=-Creating a game-=-=-=-=-=-=-=-=-=-=-=-\n");
    if(c >= 1 && c <= 3) {
        //gra z presetu
        set_values_from_preset(c, &rows, &cols, &bombs);
    } else {
        //gra niestandardowa
        printf("Enter desired board settings [rows cols bombs]:\t");
        scanf("%zu %zu %zu", &rows, &cols, &bombs);
    }


    gameBoard = make_board(rows, cols, bombs);
    print_board_stats(seed, gameBoard);
    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n");

    start_game_from_board(gameBoard);

//    switch (option) {
//        case 'l':
//            gameBoard = load_game(optarg);

//            printf("-=-=-=-=-=-=-=-=-=Loading a game from file-=-=-=-=-=-=-=-=-=-\n");
//            printf("Filename: %s\n", optarg);
//            printf("Game score: %f\n\n", gameBoard->score);
//            print_board_stats(seed, NULL, gameBoard);
//            printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n");

//            start_game_from_saved_board(gameBoard);
//            break;

//        case 'm':
//            sscanf(optarg, "%zux%zux%zu", &rows, &cols, &bombs);
//            gameBoard = make_board(rows, cols, bombs);

//            printf("-=-=-=-=-=-=-=-=-=-=-=-Creating a game-=-=-=-=-=-=-=-=-=-=-=-\n");
//            print_board_stats(seed, NULL, gameBoard);
//            printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n");

//            start_game_from_board(gameBoard);
//            break;

//        case 'p':
//            char* preset = malloc(6 * sizeof(char)); //easy, medium, hard
//            sscanf(optarg, "%s", preset);
//            set_values_from_preset(preset, &rows, &cols, &bombs);
//            gameBoard = make_board(rows, cols, bombs);

//            printf("-=-=-=-=-=-=-=-=-=-=-=-Creating a game-=-=-=-=-=-=-=-=-=-=-=-\n");
//            print_board_stats(seed, preset, gameBoard);
//            printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n");

//            free(preset);
//            start_game_from_board(gameBoard);
//            break;

//        case 's':
//                 int amountOfPlayersToShow = atoi(optarg);
//                 assert(amountOfPlayersToShow > 0);
//                 player** players = load_n_best_players_from_stats_file(&amountOfPlayersToShow);
//                 printf("%d best players:\n", amountOfPlayersToShow);
//                 print_players(players, amountOfPlayersToShow);
//                 for(int i = 0; i < amountOfPlayersToShow; i++)
//                     free_player(players[i]);
//                 free(players);
//                 break;
       
//        case 'h':
//            printf("Invalid option!\n");
//            break;

//        default:
//            printf("Unknown option: %c\n", optopt);
//            break;
//    }
    return 0;
}
