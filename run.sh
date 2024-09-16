#!/bin/sh

#
# helper script to run or debug gtk app
#

help()
{
    echo "Usage of $(basename $0) util script"
    echo "\t" "-h" "\t" "prints this message"
    echo "\t" "-d" "\t" "run debug gtk target"
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

while getopts ":hrd" opt; do
    case $opt in
        h) # display Help
            help
            exit;;
        r) # run release
            runTarget release
            exit;;
        d) # run with debugging
            GTK_DEBUG=interactive runTarget debug
            exit;;
        \?) # Invalid option
            help
            exit;;
    esac
done

echo "No option specified, try ./$(basename $0) -h"
