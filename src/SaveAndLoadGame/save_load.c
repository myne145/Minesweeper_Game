#include "../Board/board.h"
#include "save_load.h"
#include <assert.h>
#include <stdio.h> // Dla fread, fwrite
#include <stdlib.h>
#include <string.h>

#include "../Game/game_command.h"
#include "../GameStats/game_stats.h"

//Funckja zapisująca grę do bliku binarnego wybranego przez gracza
static void save_game(char* save_name,board* Board){
    assert(save_name != NULL); // Sprawdzamy czy nazwa nie jest pusta
    board_assert(Board);        // Sprawdzamy poprawność planszy
    FILE *file = fopen(save_name, "wb"); // Otwieramy plik do zapisu binarnego
    assert(file != NULL); //sprawdzamy czy udało się otworzyć plik

    int ans; // zmienna pomocnicza do sprawdzania czy operacja się powiodła

    ans = fwrite(&Board->rows, sizeof(size_t), 1, file); //zapisać liczbę wierszy
    assert(ans == 1); // sprawdzamy czy operacja się powiodła
    ans = fwrite(&Board->cols, sizeof(size_t), 1, file); //zapisać liczbę kolumn
    assert(ans == 1);
    ans = fwrite(&Board->amountOfBombs, sizeof(size_t), 1, file); //zapisać liczbę bomb na planszy
    assert(ans == 1);
    ans = fwrite(&Board->score, sizeof(float ), 1, file); //zapisać score'a
    assert(ans == 1);

    //zapisywanie czasu gry
    //zapisujemy wartości czasu gry, nie timestamp kiedy gra się zaczęła
    //po załadowaniu gry, od obecnego timestampa odejmujemy tą wartość (zaczynamy gre w przeszłości)
    //i potem liczymy już wszystko normalnie
    ans = fwrite(&Board->gameTime->tv_sec, sizeof(size_t), 1, file); //zapisywanie sekund
    assert(ans == 1);
    ans = fwrite(&Board->gameTime->tv_usec, sizeof(size_t), 1, file); //zapisywanie mikrosekund
    assert(ans == 1);

    //zapisywanie tablicy użytkownika
    for (size_t i = 0; i < Board->rows; i++) {
        for (size_t j = 0; j < Board->cols; j++){
            ans = fwrite(&(Board->P[i][j]), sizeof(int), 1, file); // zapisać wartość pola
            assert(ans == 1);
        }
    }

    //zapisywanie tablicy z rozwiązaniem
    for (size_t i = 0; i < Board->rows; i++) {
        for (size_t j = 0; j < Board->cols; j++){
            ans = fwrite(&(Board->SOLVED[i][j]), sizeof(int), 1, file); // zapisać wartość pola
            assert(ans == 1);
        }
    }
    fclose(file); //zamykamy plik
}

//Funkcja wczytująca grę z pliku binarnego wybranego przez gracza
board* load_game(char* save_name){
    assert(save_name != NULL);  // Sprawdzamy czy nazwa nie jest pusta
    FILE *file = fopen(save_name, "rb");  // Otwieramy plik do odczytu binarnego
    assert(file != NULL); //sprawdzamy czy udało się otworzyć plik


    int ans; // zmienna pomocnicza do sprawdzania czy operacja się powiodła
    size_t rows = 0, cols = 0, amountOfBombs = 0; // pomocnicze zmienne do wczytania rozmiaru planszy

    ans = fread(&rows, sizeof(size_t), 1, file); // wczytuje liczbę wierszy
    assert(ans == 1 && rows > 0); // sprawdzamy czy operacja się powiodła
    ans = fread(&cols, sizeof(size_t), 1, file); // wczytuje liczbę kolumn
    assert(ans == 1 && cols > 0); // sprawdzamy czy operacja się powiodła
    ans = fread(&amountOfBombs, sizeof(size_t), 1, file); // wczytuje liczbę bomb
    assert(ans == 1 && amountOfBombs >= 0); // sprawdzamy czy operacja się powiodła

    board* Board = make_board(rows, cols, amountOfBombs); // Tworzymy nową planszę
    board_assert(Board); // Sprawdzamy poprawność planszy

    ans = fread(&Board->score, sizeof(float), 1, file); // wczytuje score'a
    assert(ans == 1 && Board->score >= 0); // sprawdzamy czy operacja się powiodła

    //ładowanie czasu gry
    ans = fread(&Board->gameTime->tv_sec, sizeof(size_t), 1, file); //wczytywanie sekund
    assert(ans == 1); // sprawdzamy czy operacja się powiodła
    ans = fread(&Board->gameTime->tv_usec, sizeof(size_t), 1, file); //wczytywanie sekund
    assert(ans == 1); // sprawdzamy czy operacja się powiodła

    //wczytywanie tablicy użytkownika
    for (size_t i = 0; i < Board->rows; i++) {
        for (size_t j = 0; j < Board->cols; j++){
            ans = fread(&(Board->P[i][j]), sizeof(int), 1, file); // wczytuje wartość pola
            assert(ans == 1);
        }
    }

    //wczytywanie tablicy z rozwiązaniem
    for (size_t i = 0; i < Board->rows; i++) {
        for (size_t j = 0; j < Board->cols; j++){
            ans = fread(&(Board->SOLVED[i][j]), sizeof(int), 1, file); // wczytuje wartość pola
            assert(ans == 1);
        }
    }
    board_assert(Board); // Sprawdzamy poprawność planszy po wczytaniu wartosci pól
    fclose(file); //zamykamy plik
    return Board; // zwracamy wczytaną planszę
}

void save_with_exit_confirmation(board* gameBoard, char* filename)
{
    if (filename == NULL || strcmp(filename,"")==0)
    {
        fprintf(stderr, "Invalid filename\n");
        return;
    }

    //tutaj też już liczymy score'a i zapisujemy potem do pliku
    calculate_game_board_time_using_local_time(gameBoard);

    //wyrzucamy nowa linie z nazwy pliku
    for(int i = 0; i < strlen(filename); i++) {
        if(filename[i] == '\n') {
            filename[i] = (char) NULL;
        }
    }

    save_game(filename, gameBoard);
    printf("Succesfully saved the game to file %s", filename);
    int status = show_yes_no_input_field("Do you want to quit?", 0);
    if(status == 1) {
        exit(0);
    }
}
