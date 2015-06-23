#!/bin/sh

LIBDIR='../libs/avr-can-lib'
cp "$LIBDIR/can.h" "$LIBDIR/src/libcan.a" "$LIBDIR/src/config.h" .
