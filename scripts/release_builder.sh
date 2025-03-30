#!/bin/sh

set +eux

# x86_64 build
meson setup build
# arm64 build
meson setup arm-build --cross-file rpi3_cross_compile.txt

ninja -C build Ultramat.x86_64
ninja -C arm-build Ultramat.aarch64
