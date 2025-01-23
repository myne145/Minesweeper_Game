CC = gcc
CFLAGS = -lraylib -L src/RayLibGUI/Raylib/5.5/lib-I/opt/homebrew/include -Wextra -I./src


SRC_FILES = $(wildcard src/**/*.c) $(wildcard src/*.c)
OBJ_FILES = $(patsubst src/%.c,build/obj/%.o,$(SRC_FILES))

all: build/game

build/game: $(OBJ_FILES)
	@mkdir -p build
	$(CC) $(CFLAGS) $^ -o $@

build/obj/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build

run: build/game
	./build/game

run_gui: build/game
	./build/game -g

.PHONY: all clean run
