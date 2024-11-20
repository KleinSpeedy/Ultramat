#!/bin/sh

EXE=$(pwd)/build_dbg/Ultramat
GLIB_SUPP=/usr/share/glib-2.0/valgrind/glib.supp
GTK_SUPP=/usr/share/gtk-3.0/valgrind/gtk.supp

error()
{
    echo "Error: $1" >&2
    exit 1
}

if [ ! -f $EXE ]; then
    error "Could not find debug executable"
fi

if [ ! -f $GLIB_SUPP ]; then
    error "Could not find glib-2.0 valgrind suppression file"
fi

if [ ! -f $GTK_SUPP ]; then
    error "Could not find gtk-3.0 valgrind suppression file"
fi

valgrind --leak-check=full --track-origins=yes \
    --suppressions=$GLIB_SUPP \
    --suppressions=$GTK_SUPP \
    $EXE
