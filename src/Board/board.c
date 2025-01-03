#include "board.h"
#include <stdlib.h> // Dla malloc, free
#include <assert.h> // Dla assert
#include <stdio.h> // Dla printf

#define DEBUG 0

//akceptujemy pola: -4 (?), -3 (F), -2 (*), -1 (-), 0-8 (numerki)
void board_content_assert(int** arrayToCheck, const size_t rows, const size_t cols)
{
    for(size_t i = 0; i < rows; i++){
        for(size_t j = 0; j < cols; j++){
            assert(arrayToCheck[i][j] >= -4 && arrayToCheck[i][j] <= 8);
        }
    }
}
// Funkcja sprawdzająca poprawność planszy
void board_assert(board* Board){
    assert(Board != NULL); // Sprawdzamy czy wskaźnik nie jest nullem
    assert(Board->rows != 0 && Board->cols != 0); // Sprawdzamy czy liczba wierszy i kolumn jest różna od zera
    assert(Board->P != NULL); // Sprawdzamy czy wskaźnik na P nie jest nullem
    assert(Board->SOLVED != NULL); // Sprawdzamy czy wskaźnik na rozwiązanie nie jest nullem

    for(size_t i = 0; i < Board->rows; i++){
        assert(Board->P[i] != NULL); // Sprawdzamy czy wskaźnik nie jest nullem
    }

    //dla tablic P i SOLVED sprawdzamy czy mają poprawne numerki w środku
    board_content_assert(Board->P, Board->rows, Board->cols);
    board_content_assert(Board->SOLVED, Board->rows, Board->cols);

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
    newBoard->multiplier = 1; //TODO: poprawne wyliczenie multipliera
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
void free_board(board* Board){
    board_assert(Board);
    for(size_t i = 0; i < Board->rows; i++){
        free(Board->P[i]); // zwalniamy kolumny
        free(Board->SOLVED[i]); // zwalniamy kolumny
    }
    free(Board->P); // zwalniamy wiersze
    free(Board->SOLVED); // zwalniamy wiersze
    free(Board->gameTime);
    free(Board);    // zwalniamy całą plansze
}

//Funkcja pokazująca board
void print_board_game(board* Board) {
    board_assert(Board);
    for(size_t i = 0; i < Board->rows; i++){
        if( i <= 9){ // jeśli i jest jednocyfrowe to dodajemy spacje
            printf(" ");
        }
        printf("%zu. |",i);       // numer wiersza
        for(size_t j = 0; j < Board->cols; j++){
            int val = DEBUG == 0 ? Board->P[i][j] : Board->SOLVED[i][j]; // wartość pola
            switch(val){
                case -1:
                    printf(DEBUG == 0 ? " - |" : "-1 |");  // -1 to znacznik nieznanego pola
                    break;
                case 0:
                    printf(DEBUG == 0 ? "   |" : " 0 |");  // 0 to znane puste pole
                    break;
                case -2:
                    printf(DEBUG == 0 ? " * |" : "-2 |"); // -2 to znacznik miny
                    break;
                case -3:
                    printf(DEBUG == 0 ? " F |" : "-3 |"); // -3 to znacznik flagi
                    break;
                default:
                    if(val >= 1 && val <= 8){
                        printf(" %d |",val); // liczby od 1 do 8 to znaczniki ilości min w sąsiedztwie
                    }else{
                        printf(DEBUG == 0 ? " ? " : " %d ", val); // inna wartość to znacznik błędu
                    }
                    break;
            }
        }
        printf("\n"); // nowa linia (nastepny wiersz)
    }
    printf("     "); // spacje na początku (zeby wyrownac od krawedzi)
    for(size_t j = 0; j < Board->cols; j++){
        printf("----"); // linia oddzielająca pola od numeracji na dole
    }
    printf("\n"); // nowa linia (po linii oddzielającej)
    printf("    ");// spacje na początku (zeby wyrownac od krawedzi)
    for(size_t j = 0; j < Board->cols; j++){
        if( j <= 9){ // jeśli j jest jednocyfrowe to dodajemy spacje
            printf(" %zu. ",j); //numer kolumny jesli jest jednocyfrowa dajemy spacje
        }else{
            printf("%zu. ",j); //jesli dwucyfrowa to nie dajemy spacji (zeby bylo rowno)(wiekszych nie zakladamy)
        }
    }
    printf("\n");
    printf("Current score: %f\n", Board->score);
}

//helper do poprawienia indexu tablicy jeśli wychodzi po za granice
size_t get_valid_bounds(long value, board* board) {
    if(value - 1 < 0) {
        value = 0;
    }
    if(value + 1 >= board->rows) {
        value = board->rows - 1;
    }
    return value;
}

//helper do zliczenia bomb w części planszy
//start row, start col, end row, end col, board
int get_bomb_count_in_area(size_t startRow, size_t startCol, size_t endRow, size_t endCol, board* board) {
    int bombCtr = 0;
    for(size_t i = startRow; i <= endRow; i++) {
        for(size_t j = startCol; j <= endCol; j++) {
            if(board->SOLVED[i][j] == -2) {
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
void randomize_solution_to_board(board* board, size_t firstRow, size_t firstCol) {
    board_assert(board);

    if(board->amountOfBombs >= board->rows * board->cols || board->amountOfBombs < 0) {
        fprintf(stderr, "Invalid amount of bombs\n");
        exit(-1);
    }

    //licznik iteracji do tworzenia bomb
    size_t iterCounter = 0;

    //dodawanie bomb
    int bombIter = 0;
    while(bombIter < board->amountOfBombs) {
        iterCounter++;

        //warunek jeśli plansza jest nieprawidłowa - jeśli ilosc iteracji > (ilośc pól na planszy)^2 to plansza jest zła
        //wtedy to oznacza ze nie możemy znaleźć odpowiedniego miejsca na postawienia bomby na planszy
        //troszke arbitralnie postawiona granica, ale nie widze lepszej opcji
        if(iterCounter > (board->rows * board->cols) * (board->rows * board->cols)) {
            printf("Invalid board!\n");
            exit(-1);
        }

        size_t bombRow = rand() % board->rows;
        size_t bombCol = rand() % board->cols;



        //jeśli wylosowaliśmy index gdzie jest już bomba to pomijamy
        if(board->SOLVED[bombRow][bombCol] == -2) {
            continue;
        }

        //jeśli w wylosowane pole z bombą sądiaduje z pierwszym indexem to pomijamy
        //w minesweeperze jak sie zrobi 1 ruch to startowe pole zawsze jest puste
        //dzieki temu w okolicy 1 pola nie ma bomb = nie ma numerkow = pole jest puste
        if(is_index_next_to_field(firstRow, firstCol, bombRow, bombCol, board)) {
            continue;
        }

        board->SOLVED[bombRow][bombCol] = -2;

        bombIter++;
    }

    //dodawanie numerków
    for(int i = 0; i < board->rows; i++) {
        for(int j = 0; j < board->cols; j++) {
            //sprawdzamy czy pole jest bombą i pomijamy
            if(board->SOLVED[i][j] == -2) {
                continue;
            }

            //czy jak sprawdzamy ilość bomb w promieniu 1 to czy nie wyjdziemy po za granice tablicy
            size_t startRow = get_valid_bounds(i - 1, board);
            size_t startCol = get_valid_bounds(j - 1, board);
            size_t endRow = get_valid_bounds(i + 1, board);
            size_t endCol = get_valid_bounds(j + 1, board);

            //ustawianie pól na numerki
            int bombCount = get_bomb_count_in_area(startRow, startCol, endRow, endCol, board);
            board->SOLVED[i][j] = bombCount;
        }
    }
}
