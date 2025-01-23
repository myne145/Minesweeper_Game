//
// Created by antekk on 1/23/25.
//
#include "src/Board/board.h"


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
}
