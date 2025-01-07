#!/bin/bash
cc src/Board/*.c src/Game/*.c src/SaveAndLoadGame/*.c  src/RayLibGUI/*.c main.c -o game -lraylib -L src/RayLibGUI/Raylib/5.5/lib-I/opt/homebrew/include
./game
