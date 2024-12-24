//
// Created by antekk on 12/22/24.
//
#include "game.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "save.h"

static void game_iter(board* gameBoard);
static void game_loop(board* gameBoard);

void start_game_from_board(board* gameBoard) {
    //plansza która ma już wszystkie pola odkryte
    board_assert(gameBoard);

    //userBoard - plansza na której użytkownik gra, na bieżąco są odkrywane nowe pola i to ona jest drukowana
    // board* userBoard = make_board(boardToStart->rows, boardToStart->cols, boardToStart->amountOfBombs);
    // //dokładna kopia solvedBoard, tylko bez losowania
    // board_assert(userBoard);
    // print_board_game(userBoard);


    size_t row, col;
    printf("First move[row column]: ");
    assert(scanf("%zu %zu", &row, &col) == 2);
    assert(row >= 0 && row < gameBoard->rows && col >= 0 && col < gameBoard->cols);

    randomize_solution_to_board(gameBoard, row, col);
    gameBoard->P[row][col] = gameBoard->SOLVED[row][col];

    //(tymczasowo?) wyciągamy 1 iterację po za pętlę
    show_surrounding_empty_fields(row, col, gameBoard);
    print_board_game(gameBoard);

    //1 - gra w toku
    // 0 - zwróc wynik
    game_loop(gameBoard);

    free_board(gameBoard);
    // free_board(userBoard);
}



void show_surrounding_empty_fields(size_t row, size_t col, board* gameBoard) {
    //szukamy pól które musimy rekurencyjnie przeszukać
    //wybieramy obszar 3x3 chyba że jesteśmy na granicy tablicy wtedy zmneijszamy granice aby uniknac segfaulta
    size_t startRow = row - 1;
    size_t startCol = col - 1;
    //ograniczenie granic jeśli wychodzimy po za tablice
    if (startRow == - 1)
    {
        startRow = 0;
    }
    if (startCol == - 1)
    {
        startCol = 0;
    }

    size_t endRow = row + 1;
    size_t endCol = col + 1;
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
    for (size_t a = startRow; a <= endRow; a++)
    {
        for (size_t b = startCol; b <= endCol; b++)
        {
            // printf(" - Checking field (%d, %d)\n", a, b);
            //jeśli pole nie jest pustym polem to pomijamy i idziemy dalej
            //dodatkowo pole nie jest bombą - nie chcemy odkrywać bomb żeby użytkownik je widział (następny if do tego)
            if (gameBoard->SOLVED[a][b] != 0 && gameBoard->SOLVED[a][b] != -2)
            {
                //tutaj ustawiamy numerek sąsiadujący z pustym polem
                gameBoard->P[a][b] = gameBoard->SOLVED[a][b];
                continue;
            }

            //jeśli pole jest bombą też pomijamy, ale zamiast je odkrywać, pokazujemy userowi że pole jest nieznane
            if (gameBoard->SOLVED[a][b] == -2)
            {
                gameBoard->P[a][b] = -1;
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

            //jeśli żaden z powyższych warunków nie jest prawdziwy to można uznać że pole jest polem pustym i można rekurencyjnie sprawdzać dalej
            gameBoard->P[a][b] = 0;
            //rekurencja
            show_surrounding_empty_fields(a, b, gameBoard);
        }
    }
}

void place_flag(size_t row, size_t col, board* gameBoard)
{
    //gdy stawiamy flagę nigdy nie odkrywamy więcej pól
    // show_surrounding_empty_fields(row, col, gameBoard);
    if (row >= gameBoard->rows || col >= gameBoard->cols || gameBoard->P[row][col] != -1)
    {
        printf("Invalid position!\n"); //jak drukuje się to na stderr, pojawia się po następnej komendzie - dwa różne wyjścia standardowe
        return;
    }

    gameBoard->P[row][col] = -3;
}

void save_with_exit_confirmation(board* gameBoard)
{
    //maksymalna długość pliku to 50
    static int fileNameLengthMax = 50;
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
        save_with_exit_confirmation(gameBoard);
    }
    save_game(filename, gameBoard);


    printf("Do you want to quit? (y/N)\n");
    char c = fgetc(stdin);
    if (c == 'y' || c == 'Y')
    {
        exit(0);
    }
}

static void game_loop(board* gameBoard)
{
    int gameStatus = 1;
    while (gameStatus)
    {
        game_iter(gameBoard);
    }
}

static void game_iter(board* gameBoard)
{
    printf("Command (h for help):\t");
    fgetc(stdin); //usuwa znak nowej linii z poprzedniej komendy
    char command = fgetc(stdin);
    fgetc(stdin); //usuwa spacje pomiedzy komendą a danymi
    if (command == 't')
    {
        size_t row, col;
        assert(scanf("%zu %zu", &row, &col) == 2);
        place_flag(row, col, gameBoard);
    }
    else if (command == 's')
    {
        save_with_exit_confirmation(gameBoard);
    }
    else if (command == 'h')
    {
        printf("Help:\n"
               "\t• f [row][column] - places a flag in position [row][column]\n"
               "\t• r [row][column] - reveals a field in position [row][column]\n"
               "\t• ? [row][column] - marks the field at [row][column] as \"?\"\n"
               "\t• s [filename < 50 chars] - saves the current game state to specified file\n");
        return;
    }

    print_board_game(gameBoard);
}