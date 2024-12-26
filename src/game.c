//
// Created by antekk on 12/22/24.
//
#include "game.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "save.h"

static int game_iter(board* gameBoard);
static void game_loop(board* gameBoard);

void start_game_from_board(board* gameBoard) {
    //plansza która ma już wszystkie pola odkryte
    board_assert(gameBoard);

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
            //warunek 2 i 3 - pole też nie może być flagą, jako że flagi zaznaczamy tylko na planszy użytkownika,
            //sprawdzamy też i ją żeby przypadkiem nie usunąć flagi  planszy
            if (gameBoard->SOLVED[a][b] == -2 && gameBoard->P[a][b] != -3 && gameBoard->P[a][b] != -4)
            {
                gameBoard->P[a][b] = -1;
                continue;
            }

            //jeśli pole na planszy użytkownika zostało odkryte to też pomijamy je
            //również jeśli pole jest flagą lub ? którą uzytkownik postawił to pomijamy
            if (gameBoard->P[a][b] != - 1 || gameBoard->P[a][b] == -3 || gameBoard->P[a][b] == -4)
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

void place_flag(size_t row, size_t col, board* gameBoard)
{
    //gdy stawiamy flagę nigdy nie odkrywamy więcej pól

    //tutaj sprawdzamy czy index nie wychodzi po za granice tablicy i czy pole jest nieznane (-1) albo czy jest flagą
    if (row >= gameBoard->rows || col >= gameBoard->cols || (gameBoard->P[row][col] != -1 && gameBoard->P[row][col] != -3))
    {
        printf("Invalid position!\n"); //jak drukuje się to na stderr, pojawia się po następnej komendzie - dwa różne wyjścia standardowe
        return;
    }

    //jeśli w danym polu jest już flaga to usuwamy flagę i przywracamy nieznane pole
    int val = gameBoard->P[row][col] == -1 ? -3 : -1;
    gameBoard->P[row][col] = val;
}

//zwraca 1 jeśli pole nie jest bombą
//zwraca 0 jeśli pole jest bombą
int uncover_field(size_t row, size_t col, board* gameBoard)
{
    //sprawdzamy czy pole da się odsłonić - czyli czy nie wychodzi po za granice planszy i czy nie jest odsłonięte / nie jest bombą
    //możemy odsłonić tylko: nieznane pola (-1), bomby (-2), flagi (-3) i znaki zapytania (-4)
    if (row >= gameBoard->rows || col >= gameBoard->cols || (gameBoard->P[row][col] < 1 && gameBoard->P[row][col] > 8))
    {
        printf("Invalid position!\n"); //jak drukuje się to na stderr, pojawia się po następnej komendzie - dwa różne wyjścia standardowe
        return 1;
    }
    gameBoard->P[row][col] = gameBoard->SOLVED[row][col]; //odsłonięcie pola - przypisanie wartości z planszy rozwiązania do planszy usera

    //jeśli pole jest bombą i je odsłoniliśmy to przegrywamy
    if (gameBoard->SOLVED[row][col] == -2)
    {
        return 0;
    }

    //jeśli pole które odkryliśmy jest numerkiem to nie wywołujemy rekurencji
    if (gameBoard->P[row][col] >= 1 && gameBoard->P[row][col] <= 8)
    {
        return 1;
    }

    //rekurencyjna funkcja która odsłoni wszystkie sąsiadujące ze sobą pola z zeroma bombami
    show_surrounding_empty_fields(row, col, gameBoard);
    return 1;
}

//zwraca 0 jeśli gra nie wygrana (nie musi być przegrana!)
//zwraca 1 jeśli gra wygrana
static int is_game_won(board* gameBoard)
{
    //sprawdzamy czy wszystkie elementy z tablicy użytkownika są takie same jak rozwiązanie
    //jeśli tak to wygrywamy, jeśli pole to flaga to pomijamy
    //bo warunek taki nie byłby nigdy spełniony bo flagi stawiamy tylko na planszy użytkownika
    for (int i = 0; i < gameBoard->rows; i++)
    {
        for (int j = 0; j < gameBoard->cols; j++)
        {
            //warunek do flagi
            if (gameBoard->P[i][j] == -3 || gameBoard->P[i][j] == -4)
            {
                continue;
            }

            //jeśli pola nie sa takie same
            if (gameBoard->P[i][j] != gameBoard->SOLVED[i][j])
            {
                return 0;
            }
        }
    }

    return 1;
}


static void game_loop(board* gameBoard)
{
    int gameStatus = 1;
    int wasGameWon = 0;
    while (gameStatus)
    {
        gameStatus = (game_iter(gameBoard) != (wasGameWon = is_game_won(gameBoard)));
        print_board_game(gameBoard);
    }

    if (wasGameWon == 1)
    {
        printf("You won! :)\n");
        exit(0);
    }

    printf("You lost :(\n");
    exit(0);
}


//zwraca 1 jeśli gra jest w toku
//zwraca 0 jeśli mamy zwrócić wynik
int game_iter(board* gameBoard)
{
    printf("Command (h for help):\t");
    char* line = NULL;
    size_t size;

    //sprawdzamy czy udało się wczytać linie
    if (getline(&line, &size, stdin) == -1)
    {
        printf("Invalid command!\n");
        return 1;
    }


    size_t row, col;
    int iter = 1;

    //pierwszy symbol w linijce to komenda
    switch (*line)
    {
        case 'f':
            //jeśli chcemy postawić więcej niż 1 flagę na raz
            //szukamy w linijce dwóch size_t, każdy to rząd i kolumna
            while (sscanf(line + iter, "%zu %zu", &row, &col) == 2)
            {
                place_flag(row, col, gameBoard); //stawiamy flage w tych miejscach
                iter += 4; //przechodzimy 4 indexy w prawo żeby sscanf szukał tych zmiennych w dalszej czesci komendy
            }
            break;
        case 'r':
            while (sscanf(line + iter, "%zu %zu", &row, &col) == 2)
            {
                if (uncover_field(row, col, gameBoard) == 0)
                {
                    return 0; //chcemy zwrócić wynik jeśli odkryliśmy bombę
                }
                iter += 4; //przechodzimy 4 indexy w prawo żeby sscanf szukał tych zmiennych w dalszej czesci komendy
            }
            break;

        case 's':
            save_with_exit_confirmation(gameBoard);
            break;

        case 'h':
            printf("Help:\n"
            "\t• f [row1] [col1] [row2] [col2] ... [rown] [coln]- places a flag in all positions from [row1][col1] - [rown][coln]\n"
            "\t• r [row1] [col1] [row2] [col2] ... [rown] [coln] - reveals all fields in positions [row1][col1] - [rown][coln]\n"
            "\t• ? [row1] [col1] [row2] [col2] ... [rown] [coln] - marks the fields in positions [row1][col1] - [rown][coln] as \"?\"\n"
            "\t• s [filename < 50 chars] - saves the current game state to specified file\n");
            break;

        case 'd': //komenda do debugowania
            while (sscanf(line + iter, "%zu %zu", &row, &col) == 2)
            {
                printf("Element at index (%zu, %zu) is %d, in user's board is %d\n", row, col, gameBoard->SOLVED[row][col], gameBoard->P[row][col]);
                iter += 4; //przechodzimy 4 indexy w prawo żeby sscanf szukał tych zmiennych w dalszej czesci komendy
            }
            break;
    }
    printf("\n");
    return 1;
}