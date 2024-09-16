#!/bin/sh

#
# Build project in build/ direcotry
#

help()
{
    echo "Usage of $(basename $0) build script"
    echo "\t" "-h" "\t" "prints this message"
    echo "\t" "-d" "\t" "build debug build of gtk target"
    echo "\t" "-r" "\t" "build release build of gtk target"
}

build()
{
    suffix=""
    if [ $1 = "debug" ]; then
        suffix="_dbg"
    fi
    ninja -C build "Ultramat$suffix"
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
        \?) # Invalid option
            help
            exit;;
    esac
done

echo "No option specified, try ./$(basename $0) -h"
