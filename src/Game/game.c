//
// Created by antekk on 12/22/24.
//
#include "game.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "../SaveAndLoadGame/save_load.h"
#include "game_command.h"
#include "../GameStats/game_stats.h"

static int game_iter(board* gameBoard);
static void game_loop(board* gameBoard);
void update_score(size_t amountOfRevealedFields, board* gameBoard);

void start_game_from_board(board* gameBoard) {
    //plansza która ma już wszystkie pola odkryte
    board_assert(gameBoard);

    size_t row, col;
    printf("Enter your first move [row column]: ");
    assert(scanf("%zu %zu", &row, &col) == 2);
    assert(row >= 0 && row < gameBoard->rows && col >= 0 && col < gameBoard->cols);

    assert(gettimeofday(gameBoard->gameTime, NULL) == 0); //ustawianie czasu gry na obecny czas unixowy

    //wyciągamy 1 iterację po za pętlę
    size_t* revealedFields = calloc(1, sizeof(size_t));
    assert(revealedFields != NULL);

    gameBoard->P[row][col] = gameBoard->SOLVED[row][col];
    randomize_solution_to_board(gameBoard, row, col);
    show_surrounding_empty_fields(row, col, revealedFields, gameBoard);
    update_score(*revealedFields, gameBoard);
    free(revealedFields);

    game_loop(gameBoard); //free przeniesione do pętli
}

void start_game_from_saved_board(board* gameBoard)
{
    board_assert(gameBoard);

    //jako że czas przechowywany w savie jest już wyliczony, odejmujemy go od obecnego timestampa
    //zaczynamy grę w przeszłości
    struct timeval* timeval = malloc(sizeof(struct timeval));
    gettimeofday(timeval, NULL);
    gameBoard->gameTime->tv_sec = timeval->tv_sec - gameBoard->gameTime->tv_sec;
    gameBoard->gameTime->tv_usec = timeval->tv_usec - gameBoard->gameTime->tv_usec;
    free(timeval);

    print_board_game(gameBoard);
    printf("Your score: %f\n", gameBoard->score);
    printf("Command (h for help):\t");

    game_loop(gameBoard); //free przeniesione do pętli
}

void update_score(size_t amountOfRevealedFields, board* gameBoard) {
    gameBoard->score += amountOfRevealedFields * gameBoard->multiplier;
}

//tą funkcję napewno da się zoptymalizować
static void show_surrounding_empty_fields(size_t row, size_t col, size_t* buffer, board* gameBoard) {
    //szukamy pól które musimy rekurencyjnie przeszukać
    //wybieramy obszar 3x3 chyba że jesteśmy na granicy tablicy wtedy zmneijszamy granice aby uniknac segfaulta
    //można tu było użyć funkcji get_valid_bounds z board.h, ale bez tego czytelniej jest
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
    //iterowanie sie po indexach i sprawdzanie czy sa one pustymi polami jesli tak to rekurencja dalej jak nie to continue i idziemy do nastepnego
    for (size_t a = startRow; a <= endRow; a++)
    {
        for (size_t b = startCol; b <= endCol; b++)
        {
            //jeśli pole nie jest pustym polem to pomijamy i idziemy dalej
            //dodatkowo pole nie jest bombą - nie chcemy odkrywać bomb żeby użytkownik je widział (następny if do tego)
            if (gameBoard->SOLVED[a][b] != 0 && gameBoard->SOLVED[a][b] != -2)
            {
                //zwiększamy ilość odkrytych pól tylko wtedy gdy pole na planszy użytkownika
                //nie zostało jeszcze odkryte
                *buffer += gameBoard->P[a][b] == -1;

                //tutaj ustawiamy numerek sąsiadujący z pustym polem
                gameBoard->P[a][b] = gameBoard->SOLVED[a][b];
                continue;
            }

            //jeśli pole jest bombą też pomijamy, ale zamiast je odkrywać, pokazujemy userowi że pole jest nieznane
            //warunek 2 i 3 - pole też nie może być flagą, jako że flagi zaznaczamy tylko na planszy użytkownika,
            //sprawdzamy też i ją żeby przypadkiem nie usunąć flagi z planszy
            if (gameBoard->SOLVED[a][b] == -2 && gameBoard->P[a][b] != -3)
            {
                gameBoard->P[a][b] = -1;
                continue;
            }

            //jeśli pole na planszy użytkownika zostało odkryte to też pomijamy je
            //również jeśli pole jest flagą którą uzytkownik postawił to pomijamy
            if (gameBoard->P[a][b] != - 1 || gameBoard->P[a][b] == -3)
            {
                continue;
            }

            //jeśli żaden z powyższych warunków nie jest prawdziwy to można uznać że pole jest polem pustym i można rekurencyjnie sprawdzać dalej
            gameBoard->P[a][b] = 0;
            ++*buffer;
            //rekurencja
            show_surrounding_empty_fields(a, b, buffer, gameBoard);
        }
    }
}

void place_flag(size_t row, size_t col, board* gameBoard)
{
    //gdy stawiamy flagę nigdy nie odkrywamy więcej pól

    //tutaj sprawdzamy czy index nie wychodzi po za granice tablicy i czy pole jest nieznane (-1) albo czy jest flagą
    if (row >= gameBoard->rows || col >= gameBoard->cols || (gameBoard->P[row][col] != -1 && gameBoard->P[row][col] != -3))
    {
        printf("Position (%zu, %zu) is invalid\n", row, col); //jak drukuje się to na stderr, pojawia się po następnej komendzie - dwa różne wyjścia standardowe
        return;
    }

    //jeśli w danym polu jest już flaga to usuwamy flagę i przywracamy nieznane pole
    int val = gameBoard->P[row][col] == -1 ? -3 : -1;
    gameBoard->P[row][col] = val;
}

//zwraca 1 jeśli pole nie jest bombą
//zwraca 0 jeśli pole jest bombą
size_t uncover_field(size_t row, size_t col, board* gameBoard)
{
    //sprawdzamy czy pole da się odsłonić - czyli czy nie wychodzi po za granice planszy i czy nie jest odsłonięte / nie jest bombą
    //możemy odsłonić tylko: nieznane pola (-1), bomby (-2), flagi (-3) i znaki zapytania (-4)
    if (row >= gameBoard->rows || col >= gameBoard->cols || (gameBoard->P[row][col] < 1 && gameBoard->P[row][col] > 8))
    {
        printf("Position (%zu, %zu) is invalid\n", row, col); //jak drukuje się to na stderr, pojawia się po następnej komendzie - dwa różne wyjścia standardowe
        return 1;
    }

    gameBoard->P[row][col] = gameBoard->SOLVED[row][col]; //odsłonięcie pola - przypisanie wartości z planszy rozwiązania do planszy usera
    update_score(1, gameBoard);

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

    size_t amountOfRevealedFields = 0;
    //rekurencyjna funkcja która odsłoni wszystkie sąsiadujące ze sobą pola z zeroma bombami
    show_surrounding_empty_fields(row, col, &amountOfRevealedFields, gameBoard);
    update_score(amountOfRevealedFields, gameBoard);

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
            //sprawdzamy czy wszystkie flagi na planszy usera sa tylko w miejscach gdzie w rozwiązaniu są bomby
            if (gameBoard->P[i][j] == -3 && gameBoard->SOLVED[i][j] == -2)
            {
                continue;
            }

            //jeśli pola nie sa takie same lub na planszy użytkownika została odkryta bomba
            if (gameBoard->P[i][j] != gameBoard->SOLVED[i][j] || gameBoard->P[i][j] == -2)
            {
                return 0;
            }
        }
    }

    return 1;
}

void save_to_leaderboards_with_confirmation(board* gameBoard) {
    int status = show_yes_no_input_field("Save score to the leaderboards?", 1);
    if(status == 0) {
        return;
    }

    printf("Enter your name:\t");
    char* name = NULL;
    size_t size;
    //sprawdzamy czy udało się wczytać linie
    while (getline(&name, &size, stdin) == -1)
    {
        printf("Invalid name!\n");
    }

    //wyrzucamy nowa linie z imienia usera
    int nameLength = 0;
    for(; nameLength < strlen(name); nameLength++) {
        if(name[nameLength] == '\n') {
            name[nameLength] = (char) NULL;
        }
    }
    name[nameLength] = '\0';

    player* currentPlayer = create_player(name, gameBoard->score, gameBoard->gameTime);
    free(name);
    add_player_to_stats_file(currentPlayer);
    free_player(currentPlayer);
}

static void game_loop(board* gameBoard)
{
    int gameStatus;
    int wasGameWon = 0;
    while (1)
    {
        gameStatus = (game_iter(gameBoard) != (wasGameWon = is_game_won(gameBoard)));
        if(gameStatus == 0)
            break;

        print_board_game(gameBoard);
        printf("Your score: %f\n", gameBoard->score);
        printf("Command (h for help):\t");
    }

    //jesli przegralismy to odkrywamy wszystkei bomby na planszy
    if(wasGameWon == 0) {
        reveal_all_bombs(gameBoard);
    }

    print_board_game(gameBoard);
    //zatrzymujemy czas gry tutaj, bo wyszlismy z petli czyli koniec gry
    calculate_game_board_time_using_local_time(gameBoard);


    printf(wasGameWon == 1 ? "\n-=-=-=-=-=-=-=-=-=-=-=-=-You won :)=-=-=-=-=-=-=-=-=-=-=-=-=-\n" :
    "\n-=-=-=-=-=-=-=-=-=-=-=-=-You lost :(=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    printf("Your stats:\n");
    printf("Score:\t%f\n", gameBoard->score);
    printf("Time:\t%02zu.%zus\n", gameBoard->gameTime->tv_sec, gameBoard->gameTime->tv_usec / 1000);
    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n");

    if(wasGameWon) //nawet nie pytamy usera o dodanie wyniku do leaderboardsów jak przegrał
        save_to_leaderboards_with_confirmation(gameBoard);

    int amountOfPlayersToPrint = 5;
    player** players = load_n_best_players_from_stats_file(&amountOfPlayersToPrint);

    printf("%d Best players:\n", amountOfPlayersToPrint);
    printf("\tName\t\tScore\t\tTime\n");
    print_players(players, amountOfPlayersToPrint);
    for(int i = 0; i < amountOfPlayersToPrint; i++)
        free_player(players[i]);
    free(players);


    int status = show_yes_no_input_field("Try again with the same settings?", 1);
    if(status == 0) {
        free_board(gameBoard);
        exit(0);
    }

    size_t seed = time(NULL);
    srand(seed);
    board* newGameBoard = make_board(gameBoard->rows, gameBoard->cols, gameBoard->amountOfBombs);
    free_board(gameBoard);

    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=Retrying-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    print_board_stats(seed, gameBoard);
    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n");

    // printf("Retrying with size %zux%zu and %zu bombs.\nSeed: %zu\n",
    //        newGameBoard->rows, newGameBoard->cols, newGameBoard->amountOfBombs, seed);
    start_game_from_board(newGameBoard);

}

//zwraca 1 jeśli gra jest w toku
//zwraca 0 jeśli mamy zwrócić wynik
int game_iter(board* gameBoard)
{
    char* line = NULL;
    size_t size = 0;

    //sprawdzamy czy udało się wczytać linie
    if (getline(&line, &size, stdin) == -1)
    {
        printf("Invalid command!\n");
        return 1;
    }

    int commandLength = 0;
    char** command = split_command_by_spaces(line, &commandLength);
    free(line);

    //jeśli pierwsza czesc komendy ma wiecej niz 1 znak to jest zła
    //nie ma komend co mają więcej niz 1 znak na starcie, a dzięki temu ifowi switcha można użyć
    if (strlen(*command) > 1 && **command != '\n')
    {
        printf("Invalid command!\n");
        return 1;
    }

    size_t row, col;
    //pierwszy symbol w linijce to komenda
    switch (**command)
    {
        case 'f': //stawiamy flagę
            //ten piękny kod naprzemiennie ustawia wartości row i col
            //i jak obie są już ustawione to wykonuje dla nich podaną funkcję
            for (int i = 1; i < commandLength; i++)
            {
                if (i % 2 == 0)
                {
                    sscanf(command[i], "%zu", &col);
                } else
                {
                    sscanf(command[i], "%zu", &row);
                    continue;
                }

                place_flag(row, col, gameBoard);
            }
            break;
        case 'r': //odkrywamy pole
            //ten piękny kod naprzemiennie ustawia wartości row i col
            //i jak obie są już ustawione to wykonuje dla nich podaną funkcję
            for (int i = 1; i < commandLength; i++)
            {
                if (i % 2 == 0)
                {
                    sscanf(command[i], "%zu", &col);
                } else
                {
                    sscanf(command[i], "%zu", &row);
                    continue;
                }

                //jeśli klikneliśmy w bombę to kończymy grę
                if (uncover_field(row, col, gameBoard) == 0) {
                    gameBoard->score = 0; //zerujemy wynik jeśli przegraliśmy
                    free_command(command, commandLength);
                    return 0;
                }
            }
            break;

        case 's':
            save_with_exit_confirmation(gameBoard, *(command + 1));
            break;

        case 'h':
            printf("Help:\n"
            "\t• f [row1] [col1] [row2] [col2] ... [rown] [coln]- places a flag in all positions from [row1][col1] - [rown][coln]\n"
            "\t• r [row1] [col1] [row2] [col2] ... [rown] [coln] - reveals all fields in positions [row1][col1] - [rown][coln]\n"
            "\t• s [filename < 50 chars] - saves the current game state to specified file\n"
            "\t• q  - quit game without saving\n");
            break;
        case 'q':
            exit(EXIT_SUCCESS);
    }
    printf("\n");
    free_command(command, commandLength);

    return 1;
}