#!/bin/bash
cc src/Board/*.c src/Game/*.c src/SaveAndLoadGame/*.c src/GameStats/*.c src/RayLibGUI/*.c src/*.c -o game -lraylib -lm
#./game -g
