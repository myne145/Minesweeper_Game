#include "board.h"
#include <stdlib.h> // Dla malloc, free
#include <assert.h> // Dla assert
#include <stdio.h> // Dla printf

#define DEBUG 0
#define RESET_COLOR "\e[0m"


//akceptujemy pola: -3 (F), -2 (*), -1 (-), 0-8 (numerki)
void board_content_assert(int** arrayToCheck, const size_t rows, const size_t cols)
{
    for(size_t i = 0; i < rows; i++){
        for(size_t j = 0; j < cols; j++){
            assert(arrayToCheck[i][j] >= -3 && arrayToCheck[i][j] <= 8);
        }
    }
}
// Funkcja sprawdzająca poprawność planszy
void board_assert(board* gameBoard){
    assert(gameBoard != NULL); // Sprawdzamy czy wskaźnik nie jest nullem

    // Sprawdzamy czy liczba wierszy i kolumn jest różna od zera
    //i sprawdzamy czy plansza nie jest za duża - sztuczna liczba 150x150 = 22500
    if(gameBoard->rows < 1 || gameBoard->cols < 1 || gameBoard->rows * gameBoard->cols > 22500) {
        fprintf(stderr, "Invalid board dimensions!");
        exit(EXIT_SUCCESS);
    }

    assert(gameBoard->P != NULL); // Sprawdzamy czy wskaźnik na P nie jest nullem
    assert(gameBoard->SOLVED != NULL); // Sprawdzamy czy wskaźnik na rozwiązanie nie jest nullem

    for(size_t i = 0; i < gameBoard->rows; i++){
        assert(gameBoard->P[i] != NULL); // Sprawdzamy czy wskaźnik nie jest nullem
    }

    //dla tablic P i SOLVED sprawdzamy czy mają poprawne numerki w środku
    board_content_assert(gameBoard->P, gameBoard->rows, gameBoard->cols);
    board_content_assert(gameBoard->SOLVED, gameBoard->rows, gameBoard->cols);

}

float calculate_multiplier(size_t amountOfFields, size_t amountOfBombs) {
    size_t difficultyFactor = (amountOfBombs * amountOfBombs) / amountOfFields;
    return difficultyFactor;
}

// Funkcja alokująca plansze
board* make_board(size_t rows, size_t cols, size_t amountOfBombs) {
    board* newBoard = (board*)malloc(sizeof(board));
    assert(newBoard != NULL); // Sprawdzamy czy alokacja się powiodła

    assert(rows > 0 && cols > 0 && amountOfBombs>0 && amountOfBombs < (rows * cols)); // Sprawdzamy czy liczba wierszy i kolumn jest różna od zera
    
    newBoard->rows = rows;
    newBoard->cols = cols;
    newBoard->amountOfBombs = amountOfBombs;
    newBoard->score = 0;
    newBoard->multiplier = calculate_multiplier(rows * cols, amountOfBombs); //TODO: poprawne wyliczenie multipliera
    newBoard->gameTime = malloc(sizeof(struct timeval)); //przypisuje pamiec do struktury z czasem gry

    //alokacja wierszy dla obu tablic
    newBoard->P = (int**)malloc(rows * sizeof(int*));
    newBoard->SOLVED = (int**)malloc(rows * sizeof(int*));

    // Sprawdzamy czy alokacja się powiodła
    assert(newBoard->P != NULL);
    assert(newBoard->SOLVED != NULL);

    // alokacja kolumn dla obu tablic (domyslnie wypelnione zerami)
    for(size_t i = 0; i < rows; i++){
        newBoard->P[i] = (int*)calloc(cols,sizeof(int));
        newBoard->SOLVED[i] = (int*)calloc(cols,sizeof(int));

        assert(newBoard->P[i] != NULL); // Sprawdzamy czy alokacja się powiodła
    }

    for(size_t j = 0; j < newBoard->rows; j++){
        for(size_t i = 0; i < newBoard->cols; i++){
            newBoard->P[j][i] = -1; // Ustawiamy wszystkie pola na nieznane
            newBoard->SOLVED[j][i] = -1; // Ustawiamy wszystkie pola na nieznane
        }
    }
    
    return newBoard;
}

//Funkcja zwalniająca plansze
void free_board(board* gameBoard){
    board_assert(gameBoard);
    for(size_t i = 0; i < gameBoard->rows; i++){
        free(gameBoard->P[i]); // zwalniamy kolumny
        free(gameBoard->SOLVED[i]); // zwalniamy kolumny
    }
    free(gameBoard->P); // zwalniamy wiersze
    free(gameBoard->SOLVED); // zwalniamy wiersze
    free(gameBoard->gameTime);
    free(gameBoard);    // zwalniamy całą plansze
}

//Funkcja pokazująca board
void print_board_game(board* gameBoard) {
    board_assert(gameBoard);

    static char colors[8][8] = {"\e[0;34m", "\e[0;32m",
        "\e[0;33m", "\e[0;34m", "\e[0;35m", "\e[0;36m", "\e[1;34m",
    "\e[1;35m"};

    for(size_t i = 0; i < gameBoard->rows; i++){
        if( i <= 9){ // jeśli i jest jednocyfrowe to dodajemy spacje
            printf(" ");
        }
        printf("%zu. |",i);       // numer wiersza
        for(size_t j = 0; j < gameBoard->cols; j++){
            int val = DEBUG == 0 ? gameBoard->P[i][j] : gameBoard->SOLVED[i][j]; // wartość pola
            switch(val){
                case -1:
                    printf(DEBUG == 0 ? " - |" : "-1 |");  // -1 to znacznik nieznanego pola
                    break;
                case 0:
                    printf(DEBUG == 0 ? "   |" : " 0 |");  // 0 to znane puste pole
                    break;
                case -2:
                    printf(DEBUG == 0 ? "\e[1;31m * \e[0m|" : "-2 |"); // -2 to znacznik miny
                    break;
                case -3:
                    printf(DEBUG == 0 ? "\e[1;95m F \e[0m|" : "-3 |"); // -3 to znacznik flagi
                    break;
                default:
                    if(val >= 1 && val <= 8){
                        printf("%s %d %s|", colors[val - 1], val, RESET_COLOR); // liczby od 1 do 8 to znaczniki ilości min w sąsiedztwie
                    }else{
                        printf(DEBUG == 0 ? " ? " : " %d ", val); // inna wartość to znacznik błędu
                    }
                    break;
            }
        }
        printf("\n"); // nowa linia (nastepny wiersz)
    }
    printf("     "); // spacje na początku (zeby wyrownac od krawedzi)
    for(size_t j = 0; j < gameBoard->cols; j++){
        printf("----"); // linia oddzielająca pola od numeracji na dole
    }
    printf("\n"); // nowa linia (po linii oddzielającej)
    printf("    ");// spacje na początku (zeby wyrownac od krawedzi)
    for(size_t j = 0; j < gameBoard->cols; j++){
        if( j <= 9){ // jeśli j jest jednocyfrowe to dodajemy spacje
            printf(" %zu. ",j); //numer kolumny jesli jest jednocyfrowa dajemy spacje
        }else{
            printf("%zu. ",j); //jesli dwucyfrowa to nie dajemy spacji (zeby bylo rowno)(wiekszych nie zakladamy)
        }
    }
    printf("\n");
}

void print_board_stats(size_t seed, board* gameBoard) {
    printf("Settings:\n");
    printf("Dimensions: %zux%zu\n", gameBoard->rows, gameBoard->cols);
    printf("Mines: %zu\n", gameBoard->amountOfBombs);
    printf("Seed: %zu\n", seed);
    printf("Score multiplier: %f\n", gameBoard->multiplier);
}

void reveal_all_bombs(board* gameBoard) {
    for(int i = 0; i < gameBoard->rows; i++) {
        for(int j = 0; j < gameBoard->cols; j++) {
            //jeśli pole jest bomba na planszy rozwiazania to odkrywamy je na planszy uzytkownika
            gameBoard->SOLVED[i][j] == -2 ? gameBoard->P[i][j] = -2 : 0;
        }
    }
}


//helper do poprawienia indexu tablicy jeśli wychodzi po za granice
size_t get_valid_bounds(long value, board* gameBoard) {
    if(value - 1 < 0) {
        value = 0;
    }
    if(value + 1 >= gameBoard->rows) {
        value = gameBoard->rows - 1;
    }
    return value;
}

//helper do zliczenia bomb w części planszy
//start row, start col, end row, end col, gameBoard
int get_bomb_count_in_area(size_t startRow, size_t startCol, size_t endRow, size_t endCol, board* gameBoard) {
    int bombCtr = 0;
    for(size_t i = startRow; i <= endRow; i++) {
        for(size_t j = startCol; j <= endCol; j++) {
            if(gameBoard->SOLVED[i][j] == -2) {
                bombCtr++;
            }
        }
    }
    return bombCtr;
}

//a,b - TESTOWANY rząd i kolumna którą sprawdzamy czy jest index w okolicy
//row, col - rząd i kolumna który sprawdzamy czy jest w okolicy a, b
int is_index_next_to_field(size_t a, size_t b, size_t row, size_t col, board* gameBoard) {
    //granice w których szukamy pola
    size_t startRow = get_valid_bounds(a - 1, gameBoard);
    size_t startCol = get_valid_bounds(b - 1, gameBoard);
    size_t endRow = get_valid_bounds(a + 1, gameBoard);
    size_t endCol = get_valid_bounds(b + 1, gameBoard);

    //w obrębie pola (a, b) sprawdzamy czy nie znajduje się pole (row, col)
    for(size_t i = startRow; i <= endRow; i++) {
        for(size_t j = startCol; j <= endCol; j++) {
            if(i == row && j == col) { //jeśli jest to pole to zwracamy 1
                return 1;
            }
        }
    }
    //jak nie ma to zwracamy 0
    return 0;
}

/*
 * -2 = bomba
 * -1 = nieznane
 * 0 = odkryte
 * 1-8 = ilosc bomb
 */
//funkcja dodająca numerki i bomby do planszy
void randomize_solution_to_board(board* gameBoard, size_t firstRow, size_t firstCol) {
    board_assert(gameBoard);

    if(gameBoard->amountOfBombs >= gameBoard->rows * gameBoard->cols || gameBoard->amountOfBombs < 0) {
        fprintf(stderr, "Invalid amount of mines\n");
        exit(-1);
    }

    //licznik iteracji do tworzenia bomb
    size_t iterCounter = 0;

    //dodawanie bomb
    int bombIter = 0;
    while(bombIter < gameBoard->amountOfBombs) {
        iterCounter++;

        //warunek jeśli plansza jest nieprawidłowa - jeśli ilosc iteracji > (ilośc pól na planszy)^2 to plansza jest zła
        //wtedy to oznacza ze nie możemy znaleźć odpowiedniego miejsca na postawienia bomby na planszy
        //troszke arbitralnie postawiona granica, ale nie widze lepszej opcji
        if(iterCounter > (gameBoard->rows * gameBoard->cols) * (gameBoard->rows * gameBoard->cols)) {
            printf("Invalid gameBoard!\n");
            exit(-1);
        }

        size_t bombRow = rand() % gameBoard->rows;
        size_t bombCol = rand() % gameBoard->cols;



        //jeśli wylosowaliśmy index gdzie jest już bomba to pomijamy
        if(gameBoard->SOLVED[bombRow][bombCol] == -2) {
            continue;
        }

        //jeśli w wylosowane pole z bombą sądiaduje z pierwszym indexem to pomijamy
        //w minesweeperze jak sie zrobi 1 ruch to startowe pole zawsze jest puste
        //dzieki temu w okolicy 1 pola nie ma bomb = nie ma numerkow = pole jest puste
        if(is_index_next_to_field(firstRow, firstCol, bombRow, bombCol, gameBoard)) {
            continue;
        }

        gameBoard->SOLVED[bombRow][bombCol] = -2;

        bombIter++;
    }

    //dodawanie numerków
    for(int i = 0; i < gameBoard->rows; i++) {
        for(int j = 0; j < gameBoard->cols; j++) {
            //sprawdzamy czy pole jest bombą i pomijamy
            if(gameBoard->SOLVED[i][j] == -2) {
                continue;
            }

            //czy jak sprawdzamy ilość bomb w promieniu 1 to czy nie wyjdziemy po za granice tablicy
            size_t startRow = get_valid_bounds(i - 1, gameBoard);
            size_t startCol = get_valid_bounds(j - 1, gameBoard);
            size_t endRow = get_valid_bounds(i + 1, gameBoard);
            size_t endCol = get_valid_bounds(j + 1, gameBoard);

            //ustawianie pól na numerki
            int bombCount = get_bomb_count_in_area(startRow, startCol, endRow, endCol, gameBoard);
            gameBoard->SOLVED[i][j] = bombCount;
        }
    }
}
