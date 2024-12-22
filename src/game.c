//
// Created by antekk on 12/22/24.
//
#include <assert.h>
#include <stdio.h>
#include "board.h"

//TODO: wsparcie dla planszy wczytywanych z pliku
void start_game_from_board(board* startingBoard) {
    //plansza która ma już wszystkie pola odkryte
    board_assert(startingBoard);

    //userBoard - plansza na której użytkownik gra, na bieżąco są odkrywane nowe pola i to ona jest drukowana
    board* userBoard = make_board(startingBoard->rows, startingBoard->cols, startingBoard->amountOfBombs);
    //dokładna kopia solvedBoard, tylko bez losowania
    board_assert(userBoard);
    print_board_game(userBoard);


    size_t row, col;
    printf("Enter row and column[row column]: ");
    assert(scanf("%zu %zu", &row, &col) == 2);
    assert(row >= 0 && row < startingBoard->rows && col >= 0 && col < startingBoard->cols);

    randomize_board(startingBoard, row, col);
    switch (startingBoard->P[row][col]) {
    case -1:
        userBoard->P[row][col] = 0;
        break;
    }
    print_board_game(userBoard);
    // główna pętla gry
    // na koniec zwolnienie pamięci
    free_board(startingBoard);
    free_board(userBoard);
}