#!/bin/bash

set -e
mkdir -p ../build
cd ../build

# -g : debugging information(include debugging symbols)
# -Wall: Warnings all
# -Wextra: Enables even more warnings that are useful but might trigger on the valid code patterns(unused function parameters, override mismatches etc)
# -Werror: To enables all the warnings as the compilation error.
clang -g -O0 -Wall -Wextra ../code/sdl_handmade.cpp -o handmade_hero $(pkg-config --cflags --libs sdl2) -lm
echo "Build successful -> build/handmade_hero"

cd ../code
