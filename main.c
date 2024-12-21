#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "src/board.h"

int main(int argc, char** argv) {
    int option;
    size_t dims[2];
    //width,height,load,mode
    while((option = getopt(argc, argv, ":w:h:l:m:")) != -1) {
        if (option == 'w') {
            dims[0] = atol(optarg);
        } else if (option == 'h') {
            dims[1] = atol(optarg);
        } else {
            fprintf(stderr, "Invalid option!"); //TODO help message
        }
    }

    board* board = make_board(dims[0], dims[1]);

    print_board(board);


    return 0;
}
