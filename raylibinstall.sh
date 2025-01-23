#!/bin/bash
cd /opt
sudo git clone --depth 1 https://github.com/raysan5/raylib.git raylib
cd raylib/src/
make PLATFORM=PLATFORM_DESKTOP # To make the static version.
sudo make install