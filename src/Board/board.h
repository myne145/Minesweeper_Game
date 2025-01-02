#ifndef  BOARD_H
#define  BOARD_H
#include <stddef.h> // Dla size_t

/**
 * Struktura przechowujaca plansze i wlasciwości gry
 */
typedef struct Board {
    int** P;       // Tablica 2D (Pola) [rows][columns] - tablica którą "rozwiązuje" użytkownik
    int** SOLVED; // Tablica 2D (Pola) [rows][columns] - rozwiązana tablica P

    size_t rows;      // Liczba wierszy
    size_t cols;      // Liczba kolumn
    size_t amountOfBombs; //ilość bomb
    float score; //wynik użytkownika na planszy
    float multiplier; //mnożnik do wyniku dla planszy - łatwy = 1, średni = 2, trudny = 3, dla customowych liczony "manualnie"
    size_t timeInMillis; //czas gry w milisekundach
} board;

/**
 * Funkcja alokująca pamiec dla planszy
 * @param rows - liczba wierszy
 * @param cols - liczba kolumn
 * @param amountOfBombs ilość bomb
 * @return zainicjalizowana plansza
 */
board* make_board(size_t rows, size_t cols, size_t amountOfBombs);

/**
 * Funkcja zwalniająca pamiec zaalokowaną dla planszy
 * @param Board - wskaźnik na plansze
 */
void free_board(board* Board);

/**
 * Funkcja pokazująca plansze podczs gry (bez bomb)
 * @param Board - wskaźnik na plansze
 */
void print_board_game(board* Board);

/**
 * Funkcja sprawdzająca poprawność planszy
 * @param Board - wskaźnik na plansze
 */
void board_assert(board* Board);

/**
 * Funkcja wypełniająca planszę bombami i numerkami
 * @param board plansza którą chcemy wypełnić
 */
void randomize_solution_to_board(board* board,size_t firstRow, size_t firstCol);

#endif  // BOARD_H
