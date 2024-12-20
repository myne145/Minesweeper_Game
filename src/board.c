#include "board.h"
#include <stdlib.h> // Dla malloc, free
#include <assert.h> // Dla assert
#include <stdio.h> // Dla printf

// Funkcja sprawdzająca poprawność planszy
void board_assert(board* Board){
    assert(Board != NULL); // Sprawdzamy czy wskaźnik nie jest nullem
    assert(Board->rows != 0 && Board->cols != 0); // Sprawdzamy czy liczba wierszy i kolumn jest różna od zera
    assert(Board->P != NULL); // Sprawdzamy czy wskaźnik nie jest nullem
    for(size_t i = 0; i < Board->rows; i++){
        assert(Board->P[i] != NULL); // Sprawdzamy czy wskaźnik nie jest nullem
    }
    for(size_t j = 0; j < Board->rows; j++){
        for(size_t i = 0; i < Board->cols; i++){
            assert((Board->P[j][i] >= 1 && Board->P[i][j] <= 8)||(Board->P[i][j] == 0 || Board->P[i][j] == -1 || Board->P[i][j] == -2 )); // Sprawdzamy czy wartość pola jest poprawna
        }
    }
}


// Funkcja alokująca plansze
board* make_board(size_t rows, size_t cols) {
    board* newBoard = (board*)malloc(sizeof(board));
    assert(newBoard != NULL); // Sprawdzamy czy alokacja się powiodła

    assert(rows != 0 && cols != 0); // Sprawdzamy czy liczba wierszy i kolumn jest różna od zera
    
    newBoard->rows = rows;
    newBoard->cols = cols;
    newBoard->P = (int**)malloc(rows * sizeof(int*)); //alokacja tablicy wskaźników (wierszy) na tablice intów (kolumny)
    assert(newBoard->P != NULL); // Sprawdzamy czy alokacja się powiodła
    
    for(size_t i = 0; i < rows; i++){
        newBoard->P[i] = (int*)calloc(cols,sizeof(int)); // alokacja kolumn (domyslnie wypelnione zerami)
        assert(newBoard->P[i] != NULL); // Sprawdzamy czy alokacja się powiodła
    }
    
    return newBoard;
}

//Funkcja zwalniająca plansze
void free_board(board* Board){
    board_assert(Board);
    for(size_t i = 0; i < Board->rows; i++){
        free(Board->P[i]); // zwalniamy kolumny
    }
    free(Board->P); // zwalniamy wiersze
    free(Board);    // zwalniamy całą plansze
}

//Funkcja pokazująca board
void print_board(board* Board){
    board_assert(Board);
    for(size_t i = 0; i < Board->rows; i++){
        if( i <= 9){ // jeśli i jest jednocyfrowe to dodajemy spacje
            printf(" ");
        }
        printf("%zu. |",i);       // numer wiersza
        for(size_t j = 0; j < Board->cols; j++){
            int val = Board->P[i][j]; // wartość pola
            switch(val){
                case -1:
                    printf(" - |");  // -1 to znacznik nieznanego pola
                    break;
                case 0:
                    printf("   |");  // 0 to znane puste pole
                    break;
                case -2:
                    printf(" * |"); // -2 to znacznik miny
                    break;
                default:
                    if(val >= 1 && val <= 8){
                        printf(" %d |",val); // liczby od 1 do 8 to znaczniki ilości min w sąsiedztwie
                    }else{
                        printf(" ? "); // inna wartość to znacznik błędu
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
}

