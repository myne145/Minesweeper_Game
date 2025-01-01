//
// Created by antekk on 1/1/25.
//
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "game_stats.h"

#define STATS_FILE_NAME "MINESWEEPER_STATS"

player* create_player(char* name, double score, size_t timeInMillis) {
    assert(name != NULL);
    assert(score >= 0);

    player* newPlayer = malloc(sizeof(player*));
    assert(newPlayer != NULL);
    newPlayer->name = malloc(strlen(name) * sizeof(char));
    assert(newPlayer->name != NULL);

    strcpy(newPlayer->name, name);
    newPlayer->score = score;
    newPlayer->timeInMillis = timeInMillis;

    return newPlayer;
}

void add_player_to_stats_file(player* newPlayer) {
    assert(STATS_FILE_NAME != NULL);
    FILE* file = fopen(STATS_FILE_NAME, "ab");
    assert(file != NULL);

    int nameLength = strlen(newPlayer->name);

    int status;
    status = fwrite(&nameLength, sizeof(int), 1, file);
    assert(status == 1);
    status = fwrite(newPlayer->name, nameLength * sizeof(char), 1, file);
    assert(status == 1);
    status = fwrite(&newPlayer->score, sizeof(double), 1, file);
    assert(status == 1);
    status = fwrite(&newPlayer->timeInMillis, sizeof(size_t), 1, file);
    assert(status == 1);
    fclose(file);

}

player** load_n_best_players_from_stats_file(int* n) {
    player** bestPlayers = malloc(*n * sizeof(player*));
    assert(bestPlayers != NULL);
    for(int i = 0; i < *n; i++) {
        *(bestPlayers + i) = malloc(sizeof(player*));
        assert(*(bestPlayers + i) != NULL);
    }

    assert(STATS_FILE_NAME != NULL);
    FILE* file = fopen(STATS_FILE_NAME, "rb");
    if(file == NULL) {
        file = fopen(STATS_FILE_NAME, "a+b");
    }
    assert(file != NULL);

    int status;

    int playerNameLength = 0;
    char* playerName = malloc(sizeof(char*));
    double score = 0;
    size_t timeInMillis = 0;

    int i = 0;
    for(; i < *n; i++) {
        status = fread(&playerNameLength, sizeof(int), 1, file);
        if (status != 1 && playerNameLength <= 0) break;

        if(playerName != NULL)
            free(playerName);
        playerName = malloc(playerNameLength * sizeof(char));
        if(playerName == NULL) break;

        status = fread(playerName, sizeof(char) * playerNameLength, 1, file);
        if(status != 1) break;

        status = fread(&score, sizeof(double), 1, file);
        if(status != 1 || score < 0) break;

        status = fread(&timeInMillis, sizeof(size_t), 1, file);
        if(status != 1) break;

        player* newPlayer = create_player(playerName, score, timeInMillis);
        bestPlayers[i] = newPlayer;
    }
    fclose(file);

    if(playerName != NULL)
        free(playerName);

    *n = i; //długość zwróconej tablicy
    return bestPlayers;
}