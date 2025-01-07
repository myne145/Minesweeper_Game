/*
* ray.c
* Główny plik gui gry
* Autor: Oskar Przybylski
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include "Raylib/5.5/include/raylib.h" // Biblioteka RayLib
#include "ray.h"                              // Nagłówek gry
#include "ray_lib.h"                          // Funkcje pomocnicze gry


// Zmienne globalne opisujące gre
clock_t start = 0; // Czas rozpoczęcia gry
Cell board[0][0]; // Tablica komórek
int score = 0;  // Wynik
int gameOver = 0; // Czy gra się skończyła
char *mode_str = NULL; // Tryb gry (w zasadzie to jego nazwa)
int LOADED = 0;         //Flaga czy gra została wczytana
int loaded_time = 0;    //Czas zapisany do pliku zapisu potrzebny przy wczytywaniu


// Funkcja wyświetlająca listę najlepszych graczy
void leaderboard_view(void) {
    int screenWidth = 800;
    int screenHeight = 600;

    Player players[MAX_PLAYERS];
    int count = load_top_players(players);

    InitWindow(screenWidth, screenHeight, "Top Players");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Tytuł
        DrawText("TOP PLAYERS", 300, 50, 30, DARKBLUE);

        // Wyświetlanie listy graczy
        for (int i = 0; i < count; i++) {
            char text[100];
            snprintf(text, sizeof(text), "%d. %s - %d", i + 1, players[i].nick, players[i].score);
            DrawText(text, 200, 150 + i * 50, 20, BLACK);
        }

        EndDrawing();
    }

    CloseWindow();
}

// widoki
int mode_select_view(void){
    const int screenWidth = 600;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Minesweeper - Mode Selection");
    SetTargetFPS(60);

    // zmienna przechowująca wybrany tryb gry
    int modeSelected = 0; // 0 = nie wybvrano , 1 = Easy, 2 = Medium, 3 = Hard, 4 = wczytywanie z pliku
    bool startGame = false;

    // Główna pętla wyboru trybu gry
    while (!WindowShouldClose() && !startGame) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Mineswepper", screenWidth / 2 - MeasureText("Mineswepper", 30) / 2, 50, 30, BLACK);
        DrawText("by Antek Kuczynski and Oskar Przybylski", screenWidth / 2 - MeasureText("by Antek Kuczyński and Oskar Przybylski", 10) / 2, 100, 10, BLACK);

        DrawText("Select Difficulty Mode", screenWidth / 2 - MeasureText("Select Difficulty Mode", 20) / 2, 150, 20, BLACK);
        DrawText("1. Easy (9x9, 10 Mines)", screenWidth / 2 - MeasureText("1. Easy (9x9, 10 Mines)", 20) / 2, 250, 20, DARKGREEN);
        DrawText("2. Medium (16x16, 40 Mines)", screenWidth / 2 - MeasureText("2. Medium (16x16, 40 Mines)", 20) / 2, 300, 20, DARKBLUE);
        DrawText("3. Hard (16x30, 99 Mines)", screenWidth / 2 - MeasureText("3. Hard (16x30, 99 Mines)", 20) / 2, 350, 20, RED);

        DrawText("4.Load save from file...", screenWidth / 2 - MeasureText("Load save from file...", 10) / 2, 450, 10, BLACK);
        DrawText("5. Leaderboard", screenWidth / 2 - MeasureText("5. Leaderboard", 10) / 2, 470, 10, BLACK);

        if (IsKeyPressed(KEY_ONE)) {
            modeSelected = 1; // Easy mode
            startGame = true;
        } else if (IsKeyPressed(KEY_TWO)) {
            modeSelected = 2; // Medium mode
            startGame = true;
        } else if (IsKeyPressed(KEY_THREE)) {
            modeSelected = 3; // Hard mode
            startGame = true;
        }else if (IsKeyPressed(KEY_FOUR)) {
            modeSelected = 4; // Save game
            startGame = true;
        }else if(IsKeyPressed(KEY_FIVE)){
            leaderboard_view();
            CloseWindow();
        }

        EndDrawing();
    }

    CloseWindow();
    return modeSelected;
}

int save(char* filename){
    FILE* file = fopen(filename, "wb");
    if(file == NULL){
        fprintf(stderr, "Error opening file\n");
        return 0;
    }

    fwrite(&ROWS, sizeof(size_t), 1, file);
    fwrite(&COLS, sizeof(size_t), 1, file);
    fwrite(&BOMBS, sizeof(size_t), 1, file);

    fwrite(&score, sizeof(int), 1, file);

    fwrite(&board, sizeof(Cell), ROWS*COLS, file);

    int mode;
    if(strcmp(mode_str, "Easy") == 0){
        mode = 1;
    }else if(strcmp(mode_str, "Medium") == 0){
        mode = 2;
    }else if(strcmp(mode_str, "Hard") == 0){
        mode = 3;
    }

    fwrite(&mode, sizeof(int), 1, file);

    fwrite(&loaded_time, sizeof(int), 1, file);

    printf("Saved:\n");
    printf("Rows: %zu Cols: %zu Bombs: %zu \n", ROWS,COLS,BOMBS);

    printf("Mode: %d\n", mode);
    printf("score: %d\n", score);
    printf("Time: %d\n", loaded_time);

    return 1;
}

int load(char* filename){
    FILE* file = fopen(filename, "rb");
    if(file == NULL){
        fprintf(stderr, "Error opening file\n");
        return 0;
    }

    fread(&ROWS, sizeof(size_t), 1, file);
    fread(&COLS, sizeof(size_t), 1, file);
    fread(&BOMBS, sizeof(size_t), 1, file);

    fread(&score, sizeof(int), 1, file);

    fread(&board, sizeof(Cell), ROWS*COLS, file);

    int mode;
    fread(&mode, sizeof(int), 1, file);

    fread(&loaded_time, sizeof(int), 1, file);

    if(mode == 1){
        mode_str = "Easy";
    }else if(mode == 2){
        mode_str = "Medium";
    }else {
        mode_str = "Hard";
    }

    printf("Loaded:\n");
    printf("Rows: %zu Cols: %zu Bombs: %zu \n", ROWS,COLS,BOMBS);

    printf("Mode: %s\n", mode_str);

    printf("score: %d\n", score);

    printf("Time: %d\n", loaded_time);

    LOADED = 1;
    
    return 1;
}

void save_file_view(void){
    // Inicjalizacja okna
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Save game to file");

    int saved = 0;

    // Inicjalizacja zmiennych
    char* filename = (char*)malloc(100*sizeof(char));  // Tablica na nazwę pliku
    int cursorPosition = 0;  // Pozycja kursora w nazwie pliku

    SetTargetFPS(60);  // Ustawienie liczby klatek na sekundę

    while (!WindowShouldClose())
    {
        int valid = 0;

        if(cursorPosition > 98){
            fprintf(stderr, "Filename too long\n");
            exit(0);
        }
        // Obsługa wejścia
        if (IsKeyPressed(KEY_BACKSPACE) && cursorPosition > 0 && !saved) {
            filename[--cursorPosition] = '\0';  // Usunięcie ostatniego znaku
        }

        // Wpisywanie liter
        for (int key = 32; key <= 125; key++) {
            if (IsKeyPressed(key) && cursorPosition < sizeof(filename) - 1 && !saved) {
                filename[cursorPosition++] = (char)key;
                filename[cursorPosition] = '\0';  // Zapewnienie poprawnego zakończenia łańcucha
            }
        }

        if(IsKeyPressed(KEY_ENTER) && cursorPosition > 0 && !saved){
            loaded_time = (clock() - start) / 155000;
            valid = save(filename);
            saved = 1;
        }

        if(!saved){
        // Rysowanie
            BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawText("Enter save file name below:", 10, 10, 20, DARKGRAY);
            DrawText(filename, 10, 40, 20, BLACK);
            DrawText("|", 10 + MeasureText(filename, 20), 40, 20, BLACK);  // Kursor

            EndDrawing();
        }else{
        
                BeginDrawing();

                ClearBackground(RAYWHITE);

                DrawText("Saved to file:", 10, 10, 20, DARKGRAY);
                DrawText(filename, 10, 40, 20, BLACK);
    
                EndDrawing();

                if(valid){
                    exit(0);
                }
        
        }
    }

    // Zamykanie okna
    CloseWindow();
}

void load_file_view(void){
    // Inicjalizacja okna
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Load save from file");

    // Inicjalizacja zmiennych
    char* filename = (char*)malloc(100*sizeof(char));  // Tablica na nazwę pliku
    int cursorPosition = 0;  // Pozycja kursora w nazwie pliku

    SetTargetFPS(60);  // Ustawienie liczby klatek na sekundę

    while (!WindowShouldClose())
    {
        if(cursorPosition > 98){
            fprintf(stderr, "Filename too long\n");
            exit(0);
        }
        // Obsługa wejścia
        if (IsKeyPressed(KEY_BACKSPACE) && cursorPosition > 0) {
            filename[--cursorPosition] = '\0';  // Usunięcie ostatniego znaku
        }

        // Wpisywanie liter
        for (int key = 32; key <= 125; key++) {
            if (IsKeyPressed(key) && cursorPosition < sizeof(filename) - 1) {
                filename[cursorPosition++] = (char)key;
                filename[cursorPosition] = '\0';  // Zapewnienie poprawnego zakończenia łańcucha
            }
        }

        if(IsKeyPressed(KEY_ENTER) && cursorPosition > 0){
            int valid  = load(filename);
            if(valid){
                break;
            }
        }

        // Rysowanie
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("Enter save file name below:", 10, 10, 20, DARKGRAY);
        DrawText(filename, 10, 40, 20, BLACK);
        DrawText("|", 10 + MeasureText(filename, 20), 40, 20, BLACK);  // Kursor

        EndDrawing();
    }

    // Zamykanie okna
    CloseWindow();
}

void main_game_view(void) {
    
    const int screenWidth = COLS * CELL_SIZE;
    const int screenHeight = ROWS * CELL_SIZE;

    InitWindow(screenWidth, screenHeight, "Minesweeper");
    SetTargetFPS(60);

    if(!LOADED){
        InitBoard(board);
    }
    int win = 0;

    score = 1000 * BOMBS;
    

    while (!WindowShouldClose() && !gameOver && !win) {
        // Obsługa myszy
        if (!gameOver && !win) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mouse = GetMousePosition();
                int col = (mouse.x ) / CELL_SIZE;
                int row = (mouse.y - 40) / CELL_SIZE;
                if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
                    RevealCell(board, row, col, &gameOver);
                }
            } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                Vector2 mouse = GetMousePosition();
                int col = (mouse.x ) / CELL_SIZE;
                int row = (mouse.y - 40) / CELL_SIZE;
                if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
                    ToggleFlag(board, row, col);
                }
            }else if(IsKeyPressed(KEY_S)){
                save_file_view();
            }
        }

        score -= (clock() - start)/(1550000);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        win = is_game_won(board);

        DrawBoard(board, gameOver,mode_str,score, (clock() - start)/(155000)+loaded_time);

        EndDrawing();
    }

    if(gameOver){
        int time = (clock() - start) / 155000;

        InitWindow(screenWidth, screenHeight, "Game Over");
        while(!WindowShouldClose()){
        
            BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText("Game over Christopher..", screenWidth/5  -10 , screenHeight/5   , 10, RED);
            DrawText(TextFormat("Time: %d", time), screenWidth/5  -10 , screenHeight/5 + 50, 10, RED);
            DrawText(TextFormat("Score: %d", score), screenWidth/5  -10 , screenHeight/5 + 100, 10, RED);
            EndDrawing();
        }
    }

    if(win){

        // Inicjalizacja zmiennych
        char* nick = (char*)malloc(60*sizeof(char));  // Tablica na nazwę pliku
        int cursorPosition = 0;  // Pozycja kursora w nazwie pliku
        int time = (clock() - start) / 155000;

        InitWindow(screenWidth, screenHeight, "You win!!!");
        while(!WindowShouldClose()){ 

            if(cursorPosition > 58){
                fprintf(stderr, "Nick too long\n");
                exit(0);
            }
             // Obsługa wejścia
             if (IsKeyPressed(KEY_BACKSPACE) && cursorPosition > 0) {
                 nick[--cursorPosition] = '\0';  // Usunięcie ostatniego znaku
            }

              // Wpisywanie liter
             for (int key = 32; key <= 125; key++) {
                 if (IsKeyPressed(key) && cursorPosition < sizeof(nick) - 1) {
                        nick[cursorPosition++] = (char)key;
                        nick[cursorPosition] = '\0';  // Zapewnienie poprawnego zakończenia łańcucha
               }
            }

             if(IsKeyPressed(KEY_ENTER) && cursorPosition > 0){
                    save_top_player(nick, score);
                    leaderboard_view();
             }
        
            BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText("You Win !!!", screenWidth/5  -10 , screenHeight/5   , 10, GREEN);
            DrawText(TextFormat("Time: %d", time), screenWidth/5  -10 , screenHeight/5 + 50, 10, RED);
            DrawText(TextFormat("Score: %d", score), screenWidth/5  -10 , screenHeight/5 + 100, 10, RED);

            DrawText("Enter your name below:", 10, 10, 20, DARKGRAY);
            DrawText(nick, 10, 40, 20, BLACK);
            DrawText("|", 10 + MeasureText(nick, 20), 40, 20, BLACK);  // Kursor

            EndDrawing();
        }
    }
    

    CloseWindow();
}

void gui(void){
    int mode = mode_select_view();

    assert(mode == 1 || mode == 2 || mode == 3 || mode == 4);
    mode_str = (mode == 1) ? "Easy" : (mode == 2) ? "Medium" : (mode==3) ?  "Hard": "**loaded**";

    if(mode == 4){
        Cell board[ROWS][COLS];
        load_file_view(); 
           
    }else{
        if(mode == 1){
            ROWS = 9;
            COLS = 9;
            BOMBS = 10;
        }else if(mode == 2){
            ROWS = 16;
            COLS = 16;
            BOMBS = 40;
        }else if(mode == 3){
            ROWS = 16;
            COLS = 30;
            BOMBS = 99;
        }
        clock_t start = clock();
        Cell board[ROWS][COLS];
    }

    main_game_view();
}