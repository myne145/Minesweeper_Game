//
// Created by antekk on 1/23/25.
//
#include "src/Board/board.h"
#include "src/SaveAndLoadGame/save_load.h"


int main(int argc, char** argv) {
    board* gameBoard = make_board(10,10,50);
    board_assert(gameBoard);

    //Randomized board test
    randomize_solution_to_board(gameBoard, 0, 0);
    print_board_game(gameBoard);

    //Invalid value test
//    gameBoard->P[0][0] = -10;
//    print_board_game(gameBoard);


    //Bombs placement test
    reveal_all_bombs(gameBoard);
    print_board_game(gameBoard);

    //Free board test
    free_board(gameBoard);


    //Loading board from file test
    board* loadedBoard = load_game("test_game.bin");
    board_assert(loadedBoard);
    print_board_game(loadedBoard);

    reveal_all_bombs(loadedBoard);
    print_board_game(loadedBoard);

}
