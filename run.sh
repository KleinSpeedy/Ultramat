#!/bin/sh

#
# helper script to run or debug gtk app
#

help()
{
    echo "Usage of $(basename $0) util script"
    echo -e "\t" "-h" "\t" "prints this message"
    echo -e "\t" "-d" "\t" "run debug gtk target"
    echo -e "\t" "-r" "\t" "run release gtk target"
}

runTarget()
{
    WHERE=$1
    ./build/$WHERE/ultramat
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
    esac
done

echo "No option specified, try ./$(basename $0) -h"
