#!/bin/sh

set +eux

# x86_64 release build
meson setup build
meson setup arm-build --cross-file rpi3_cross_compile.txt

ninja -C build Ultramat.x86_64
ninja -C arm-build Ultramat.arm64
