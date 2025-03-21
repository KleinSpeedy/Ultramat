#!/bin/sh

#
# helper script to run or debug gtk app
#

help()
{
    echo "Usage of $(basename $0) util script"
    echo "\t" "-h" "\t" "prints this message"
    echo "\t" "-d" "\t" "run debug gtk target"
    echo "\t" "-g" "\t" "run debug gtk target with gdb"
    echo "\t" "-r" "\t" "run release gtk target"
}

runTarget()
{
    suffix=""
    if [ $1 = "debug" ]; then
        suffix="_dbg"
    fi
    ./build/Ultramat$suffix
}

while getopts ":hrdg" opt; do
    case $opt in
        h) # display Help
            help
            exit;;
        r) # run release
            runTarget release
            exit;;
        d) # run with debugging
            GTK_DEBUG=interactive G_DEBUG=fatal-warnings runTarget debug
            exit;;
        g) # run with gdb
            GTK_DEBUG=interactive G_DEBUG=fatal-warnings gdb ./build/Ultramat_dbg
            exit;;
        \?) # Invalid option
            help
            exit;;
    esac
done

echo "No option specified, try ./$(basename $0) -h"
