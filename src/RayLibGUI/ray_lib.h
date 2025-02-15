#ifndef RAY_LIB_H
#define RAY_LIB_H
#include <stdio.h>
#include <time.h>

// Rodzaje stanów komórki
typedef enum { HIDDEN, REVEALED, FLAGGED } CellState;

// Struktura przechowująca informacje o komórce planszy
typedef struct {
    int isBomb; // 1 jeśli komórka zawiera bombę, 0 w przeciwnym przypadku
    int neighborBombs; // Liczba bomb w sąsiednich komórkach
    CellState state; // Stan komórki
} Cell;

// Mnoznik wielkosci elementow gry
#define SCALE 2

// Flaga trybu debugowania
#define DEBUG 0

// Rozmiar komórki planszy
#define CELL_SIZE (SCALE*40)

// Zmienne globalne określające rozmiar planszy
extern size_t ROWS;
extern size_t COLS;

// Liczba bomb na planszy
extern size_t BOMBS;

// Tabela najlepszych graczy
#define MAX_PLAYERS 5
#define NICK_MAX_LENGTH 50
#define FILE_NAME "top_players.bin"

// Struktura przechowująca nick i wynik gracza
typedef struct {
    char nick[NICK_MAX_LENGTH];
    int score;
} Player;

/*
* Funkcja inicjalizująca planszę
*/
void RevealCell(Cell board[ROWS][COLS], int row, int col, int *gameOver);

/*
*  Funkcja ustawiająca flagę na komórce
*/
void ToggleFlag(Cell board[ROWS][COLS], int row, int col);

/*
*   Funkcja obliczająca liczbę bomb w sąsiednich komórkach
*/
void CalculateNeighbors(Cell board[ROWS][COLS]);


// Funkcja losowo rozmieszczająca bomby na planszy
// Losuje współrzędne komórek, aż do momentu, gdy wszystkie bomby zostaną rozmieszczone
// Predefiniowana ilość bomb jest zdefiniowana jako BOMBS, komórki są losowane aż do momentu, gdy wszystkie bomby zostną rozmieszczone
void PlaceBombs(Cell board[ROWS][COLS],int firstrow,int firstcol);

// Funkcja inicjalizująca planszę gry
void InitBoard(Cell board[ROWS][COLS],int firstrow, int firstcol);

// Funkcja rysująca planszę gry dzięki bibliotece Raylib
void DrawBoard(Cell board[ROWS][COLS], int gameOver, const char* mode, int score, int gameTime);

// Funkcja zapisująca stan gry
int SaveGame(const char *filename,size_t ROWS,size_t COLS,size_t BOMBS ,clock_t start, Cell board[ROWS][COLS], int score, int gameOver);

// Funkcja do odczytu stanu gry
int LoadGame(const char *filename, size_t* pROWS,size_t* pCOLS,size_t* BOMBS ,clock_t *start, Cell board[ROWS][COLS], int *score, int *gameOver);

// Funkcja sprawdzająca czy gra została wygrana
int IsGameWon(Cell board[ROWS][COLS]);

// Funkcja zapisująca top 5 graczy do pliku 
void SaveTopPlayer(const char *nick, int score);

// Funkcja odczytująca najlepszych graczy z pliku
int LoadTopPlayers(Player players[]);

// Funkcja zliczajaca odkryte pola planszy
int CountRevealedCell(Cell board[ROWS][COLS]);

#endif //RAY_LIB_H