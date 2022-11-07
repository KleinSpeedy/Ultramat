#!/bin/sh

# build process
#cmake -Bbuild -H. -GNinja
cmake -B build -S .
cmake --build build