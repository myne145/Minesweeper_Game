//
// Created by antekk on 12/29/24.
//

#include <assert.h>
#include <malloc.h>
#include <string.h>
#include "game_command.h"

int show_yes_no_input_field(char* text, int defaultYes) {
    if(defaultYes != 1 && defaultYes != 0)
        return -1;
    char* msg = malloc(2 * sizeof(char ));
    msg[0] = (char) ('y' - (defaultYes * 32));
    msg[1] = (char) ('N' + (defaultYes * 32));
    printf("%s (%c, %c)\t", text, msg[0], msg[1]);
    free(msg);
    char c = fgetc(stdin);
    if(c == '\n')
        return defaultYes;
    fgetc(stdin);
    if(c == 'y' || c == 'Y')
        return 1;
    return 0;
}

char** split_command_by_spaces(char* line, int* commandLength) {
//dzielimy komendę po spacjach na tablice stringów
    char** command = malloc(strlen(line) * sizeof(char*));
    assert(command != NULL && "Failed to allocate memory for the command string");

    command[0] = malloc(20 * sizeof(char)); //wyciągnięta 1 iteracja po za pętlę
    assert(command[0] != NULL && "Failed to allocate memory for the 0th command element");


    *commandLength = 0;
    int tempElementIndex = 0;
    for (int i = 0; i < strlen(line); i++)
    {
        if (line[i] == ' ')
        {
            command[*commandLength][tempElementIndex] = '\0';
            tempElementIndex = 0;
            ++*commandLength;

            command[*commandLength] = malloc(20 * sizeof(char)); //maksymalnie 20 znaków w jednej części komendy
            assert(command[*commandLength] != NULL && "Failed to allocate memory for the >0th command element");
            continue;
        }
        command[*commandLength][tempElementIndex] = line[i];
        tempElementIndex++;
    }
    ++*commandLength;

    return command;
}

void free_command(char** command, int commandLength) {
    for(int i = 0; i < commandLength; i++) {
        free(command[i]);
    }
    free(command);
}