/*
* ray_lib.c
* Pomocniczy plik gui gry
* Autor: Oskar Przybylski
*/

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include <string.h>

#include <raylib.h>
#include "ray_lib.h"

size_t ROWS,COLS,BOMBS;

// Funkcja rekurencyjnie odkrywająca komórki wokół komórki o współrzędnych (row, col). Zmienia gameOver na 1 jeśli odkryta komórka zawiera bombę
void RevealCell(Cell board[ROWS][COLS], int row, int col, int *gameOver) {
    // Sprawdzenie czy współrzędne są poprawne
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS || board[row][col].state != HIDDEN) return;

    // Jeśli komórka jest oznaczona flagą, nie odkrywamy jej
    if (board[row][col].state == FLAGGED) return;

    // Odkrycie komórki
    board[row][col].state = REVEALED;

    // Jeśli komórka zawiera bombę, gra się kończy
    if (board[row][col].isBomb) {
        *gameOver = 1; // Przegrana
        return;
    }

    // Jeśli komórka nie zawiera bomb, odkrywamy sąsiednie komórki
    if (board[row][col].neighborBombs == 0) {
        // Pętla przechodzi przez wszystkie sąsiednie komórki
        for (int dr = -1; dr <= 1; dr++) {  
            // Pętla od -1 do 1 dla wiersza: (-1, 0, 1) — aby obejmować komórki powyżej, samej komórki i poniżej
            for (int dc = -1; dc <= 1; dc++) {
                // Pętla od -1 do 1 dla kolumny: (-1, 0, 1) — aby obejmować komórki po lewej, samej komórki i po prawej
                // Wywołanie funkcji RevealCell dla każdej sąsiedniej komórki
                RevealCell(board, row + dr, col + dc, gameOver);
            }
        }
    }
}

// Funkcja oznaczająca komórkę flagą lub usuwająca flagę
void ToggleFlag(Cell board[ROWS][COLS], int row, int col) {
    // Jeśli komórka jest zakryta, oznaczamy ją flagą
    if (board[row][col].state == HIDDEN) {
        board[row][col].state = FLAGGED;
    // Jeśli komórka jest oznaczona flagą, usuwamy flagę
    } else if (board[row][col].state == FLAGGED) {
        board[row][col].state = HIDDEN;
    }
}

// Funkcja obliczająca liczbę bomb w sąsiednich komórkach
void CalculateNeighbors(Cell board[ROWS][COLS]) {
    // Pętle przechodzą przez wszystkie komórki planszy
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            // Jeśli komórka zawiera bombę, pomijamy ją
            if (board[row][col].isBomb) continue;
            // Liczba bomb w sąsiednich komórkach
            int bombs = 0;
            // Pętle przechodzą przez wszystkie sąsiednie komórki
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    int nr = row + dr; // Wiersz sąsiedniej komórki
                    int nc = col + dc; // Kolumna sąsiedniej komórki
                    // Sprawdzenie czy sąsiednie komórki zawierają bombę
                    if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && board[nr][nc].isBomb) {
                        // Jeśli sąsiednia komórka zawiera bombę, zwiększamy liczbę
                        bombs++;
                    }
                }
            }
            // Zapisanie liczby bomb w sąsiednich komórkach w komórce
            board[row][col].neighborBombs = bombs;
        }
    }
}

// Funkcja losowo rozmieszczająca bomby na planszy
// Losuje współrzędne komórek, aż do momentu, gdy wszystkie bomby zostaną rozmieszczone
// Predefiniowana ilość bomb jest zdefiniowana jako BOMBS, komórki są losowane aż do momentu, gdy wszystkie bomby zostną rozmieszczone
void PlaceBombs(Cell board[ROWS][COLS]) {
    int placedBombs = 0;
    while (placedBombs < BOMBS) {
        int row = rand() % (ROWS-1);
        int col = rand() % (COLS-1);
        // Jeśli komórka nie zawiera bomby, umieszczamy w niej bombę
        if (!board[row][col].isBomb) {
            board[row][col].isBomb = 1;
            placedBombs++;
        }
    }
}

int CountRevealedCell(Cell board[ROWS][COLS]){
    int counter = 0;

    for(int row = 0; row < ROWS; row++){
        for(int col = 0; col < COLS; col++){
            if(board[row][col].state == REVEALED){
                counter++;
            }        
        }
    }

    return counter;
}

// Funkcja inicjalizująca planszę gry
void InitBoard(Cell board[ROWS][COLS]) {
    // Inicjalizacja generatora liczb losowych
    srand(time(NULL));
    // Pętle przechodzą przez wszystkie komórki planszy
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            // Ustawienie początkowych wartości komórek
            board[row][col].isBomb = 0;
            board[row][col].neighborBombs = 0;
            board[row][col].state = HIDDEN;
        }
    }
    // Umieszczenie bomb na planszy
    PlaceBombs(board);
    // Obliczenie liczby bomb w sąsiednich komórkach
    CalculateNeighbors(board);
}

// Funkcja rysująca planszę gry dzięki bibliotece Raylib
void DrawBoard(Cell board[ROWS][COLS], int gameOver, const char* mode, int score, int gameTime) {
    // Wyświetlanie tabelki na górze
    int tableHeight = SCALE * 40;
    DrawRectangle(0, 0, COLS * CELL_SIZE, tableHeight, DARKGRAY);
    DrawRectangleLines(0, 0, COLS * CELL_SIZE, tableHeight, BLACK);

    // Wyświetlanie tekstu w tabelce
    DrawText(TextFormat("Mode: %s", mode),SCALE *  10,SCALE *  10, SCALE * 20, WHITE);
    DrawText(TextFormat("Score: %d", score),SCALE *  150,SCALE *  10, SCALE * 20, WHITE);
    DrawText(TextFormat("Time: %d", gameTime),SCALE *  275,SCALE *  10, SCALE * 20, WHITE);

    // Rysowanie planszy gry
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            int x = col * CELL_SIZE;                // Współrzędna x komórki
            int y = row * CELL_SIZE + tableHeight; // Dodanie wysokości tabelki

            // Utworzenie prostokąta reprezentującego komórkę
            Rectangle cell = { x, y, CELL_SIZE, CELL_SIZE };

            // Jeśli komórka jest zakryta, rysujemy ją na szaro
            if (board[row][col].state == HIDDEN) {
                DrawRectangleRec(cell, LIGHTGRAY);
                if(board[row][col].isBomb){
                    if(0)DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, WHITE);
                }
                DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, DARKGRAY);
            // Jeśli komórka jest oznaczona flagą, rysujemy ją na niebiesko
            } else if (board[row][col].state == FLAGGED) {
                DrawRectangleRec(cell, SKYBLUE);
                DrawText("F", x + CELL_SIZE / 4, y + CELL_SIZE / 4,SCALE *  20, BLACK);
            // Jeśli komórka jest odkryta,
            } else if (board[row][col].state == REVEALED) {
                // i ma bombę, rysujemy ją na czerwono
                if (board[row][col].isBomb) {
                    DrawRectangleRec(cell, RED);
                    DrawText("*", x + CELL_SIZE / 4, y + CELL_SIZE / 4,SCALE *  20, BLACK);
                // jeśli nie ma bomby, rysujemy ją na biało
                } else {
                    DrawRectangleRec(cell, RAYWHITE);
                    // Jeśli komórka ma sąsiadujące bomby, rysujemy liczbę
                    if (board[row][col].neighborBombs > 0) {
                        DrawText(TextFormat("%d",board[row][col].neighborBombs), x + CELL_SIZE / 4, y + CELL_SIZE / 4,SCALE *  20, DARKGRAY);
                    }
                }
                // Rysowanie obramowania komórki
                DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, DARKGRAY);
            }
        }
    }
}

// Funkcja sprawdzająca, czy gra została wygrana
int is_game_won(Cell board[ROWS][COLS]){
    // Iteracja po wszystkich komórkach
    for(int row = 0; row < ROWS; row++){
        for(int col = 0; col < COLS; col++){
            // Jeśli komórka zawiera bombę, ale nie jest oznaczona flagą, gra nie jest wygrana
            if(board[row][col].isBomb){
                if(board[row][col].state != FLAGGED){
                    return 0;
                }
            }
            // Jeśli komórka która nie zawiera bomby jest zakryta, gra nie jest wygrana
            if(board[row][col].state == HIDDEN && !board[row][col].isBomb){
                //return 0;
            }
        }
    }
    // Jeśli wszystkie bomby są oznaczone flagami, gra jest wygrana
    return 1;
}

// Funkcja porównująca wyniki graczy (do sortowania)
int compare_scores(const void *a, const void *b) {
    Player *playerA = (Player *)a;
    Player *playerB = (Player *)b;
    return playerB->score - playerA->score; // Sortowanie malejąco
}

// Funkcja zapisująca top 5 graczy do pliku binarnego
void save_top_player(const char *nick, int score) {
    Player players[MAX_PLAYERS + 1]; // 6 miejsc, żeby uwzględnić nowego gracza
    int count = 0;

    // Odczyt istniejących wyników z pliku
    FILE *file = fopen(FILE_NAME, "rb");
    if (file != NULL) {
        count = fread(players, sizeof(Player), MAX_PLAYERS, file);
        fclose(file);
    }

    // Dodanie nowego gracza do listy
    if (count < MAX_PLAYERS || score > players[MAX_PLAYERS - 1].score) {
        strcpy(players[count].nick, nick);
        players[count].score = score;
        count++;
    }

    // Posortowanie graczy według wyników
    qsort(players, count, sizeof(Player), compare_scores);

    // Zapisanie tylko top 5 wyników do pliku
    file = fopen(FILE_NAME, "wb");
    if (file == NULL) {
        perror("Błąd podczas otwierania pliku");
        return;
    }
    fwrite(players, sizeof(Player), count < MAX_PLAYERS ? count : MAX_PLAYERS, file);
    fclose(file);
}

// Funkcja odczytująca najlepszych graczy z pliku
int load_top_players(Player players[]) {
    FILE *file = fopen(FILE_NAME, "rb");
    if (file == NULL) {
        return 0; // Jeśli plik nie istnieje, zwracamy 0 graczy
    }
    int count = fread(players, sizeof(Player), MAX_PLAYERS, file);
    fclose(file);
    return count;
}