#!/bin/sh

#
# Build project in build/ direcotry
#

help()
{
    echo "Usage of $(basename $0) build script"
    echo -e "\t" "-h" "\t" "prints this message"
    echo -e "\t" "-d" "\t" "build debug build of gtk target"
    echo -e "\t" "-r" "\t" "build release build of gtk target"
}

build()
{
    echo "Building $1 target"
    cmake --build build/$1
}

while getopts ":hrd" opt; do
    case $opt in
        h) # display Help
            help
            exit;;
        r) # build release
            build "release"
            exit;;
        d) # build debug
            build "debug"
            exit;;
    esac
done

echo "No option specified, try ./$(basename $0) -h"
