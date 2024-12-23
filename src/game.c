//
// Created by antekk on 12/22/24.
//
#include "game.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "save.h"

static void game_loop(board* userBoard, board* filledBoard);

void start_game_from_board(board* filledBoard) {
    //plansza która ma już wszystkie pola odkryte
    board_assert(filledBoard);

    //userBoard - plansza na której użytkownik gra, na bieżąco są odkrywane nowe pola i to ona jest drukowana
    board* userBoard = make_board(filledBoard->rows, filledBoard->cols, filledBoard->amountOfBombs);
    //dokładna kopia solvedBoard, tylko bez losowania
    board_assert(userBoard);
    print_board_game(userBoard);


    size_t row, col;
    printf("First move[row column]: ");
    assert(scanf("%zu %zu", &row, &col) == 2);
    assert(row >= 0 && row < filledBoard->rows && col >= 0 && col < filledBoard->cols);

    randomize_board(filledBoard, row, col);
    userBoard->P[row][col] = filledBoard->P[row][col];

    //(tymczasowo?) wyciągamy 1 iterację po za pętlę
    show_surrounding_empty_fields(filledBoard, userBoard, row, col);
    print_board_game(userBoard);

    game_loop(userBoard, filledBoard);

    free_board(filledBoard);
    free_board(userBoard);
}



void show_surrounding_empty_fields(board* filledBoard, board* gameBoard, int row, int col) {
    //szukamy pól które musimy rekurencyjnie przeszukać
    //wybieramy obszar 3x3 chyba że jesteśmy na granicy tablicy wtedy zmneijszamy granice aby uniknac segfaulta
    int startRow = row - 1;
    int startCol = col - 1;
    //ograniczenie granic jeśli wychodzimy po za tablice
    if (startRow == - 1)
    {
        startRow = 0;
    }
    if (startCol == - 1)
    {
        startCol = 0;
    }

    int endRow = row + 1;
    int endCol = col + 1;
    //to samo tylko dla wartości końcowych
    if (endRow >= gameBoard->rows)
    {
        endRow = gameBoard->rows - 1;
    }
    if (endCol >= gameBoard->cols)
    {
        endCol = gameBoard->cols - 1;
    }
    // printf("(%d, %d) - startRow: %d, startCol: %d, endRow: %d, endCol: %d\n", row, col, startRow, startCol, endRow, endCol);
    //iterowanie sie po indexach i sprawdzanie czy sa one pustymi polami jesli tak to rekurencja dalej jak nie to continue i idziemy do nastepnego
    for (int a = startRow; a <= endRow; a++)
    {
        for (int b = startCol; b <= endCol; b++)
        {
            // printf(" - Checking field (%d, %d)\n", a, b);
            //jeśli pole nie jest pustym polem to pomijamy i idziemy dalej
            if (filledBoard->P[a][b] != 0)
            {
                //tutaj ustawiamy numerek sąsiadujący z pustym polem
                gameBoard->P[a][b] = filledBoard->P[a][b];
                continue;
            }
            //jeśli pole na planszy użytkownika zostało odkryte to też pomijamy je
            if (gameBoard->P[a][b] != - 1)
            {
                continue;
            }

            //pomijamy pole z którego funkcja została wywowałana
            if (a == row && b == col)
            {
                continue;
            }
            gameBoard->P[a][b] = 0;
            show_surrounding_empty_fields(filledBoard, gameBoard, a, b);
        }
    }
}

void touch(size_t row, size_t col, board* userBoard, board* filledBoard)
{
    printf("touch %zu %zu\n", row, col);
}

void save_with_exit_confirmation(board* userBoard, board* filledBoard)
{
    //maksymalna długość pliku to 50
    static int fileNameLengthMax = 2;
    char filename[fileNameLengthMax];
    char buf;
    int i = 0;
    while ((buf = fgetc(stdin)) != '\n' && i < fileNameLengthMax - 1)
    {
        filename[i] = buf;
        i++;
    }
    filename[i] = '\0';
    if (filename[0] == '\0')
    {
        fprintf(stderr, "Invalid filename\n");
        save_with_exit_confirmation(userBoard, filledBoard);
    }
    save_game(filename, userBoard);


    printf("Do you want to quit? (y/N)\n");
    char c = fgetc(stdin);
    if (c == 'y' || c == 'Y')
    {
        exit(0);
    }
}
static void game_loop(board* userBoard, board* filledBoard)
{
    printf("Command:\t");
    fgetc(stdin);
    char command = fgetc(stdin);

    if (command == 't')
    {
        size_t row, col;
        assert(scanf("%zu %zu", &row, &col) == 2);
        touch(row, col, userBoard, filledBoard);
    }
    else if (command == 's')
    {
        save_with_exit_confirmation(userBoard, filledBoard);
    }

}