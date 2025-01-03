#!/bin/bash
cc src/Board/*.c src/Game/*.c src/SaveAndLoadGame/*.c  src/RayLibGUI/*.c main.c -o game -lraylib -L/opt/homebrew/lib -I/opt/homebrew/include -framework OpenGL -framework Cocoa -framework IOKit
./game
