#!/bin/sh

#
# Bootstrap the Ultramat GTK Project
#

HERE=$PWD

makeTarget()
{
    DEBUG=$1
    BUILD_DIR=$2

    echo "===================="
    echo "Building target: $BUILD_DIR"
    echo "===================="

    cmake \
        -B build/$2 \
        -S $HERE \
        -DULTRA_BUILD_DEBUG:NUMBER=$DEBUG
}

makeTarget 0 release
makeTarget 1 debug
