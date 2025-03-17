#!/bin/sh

#
# Testing for serial communication
#

socat -d -d pty,rawer,echo=0,link=/tmp/ttyArduino pty,rawer,echo=0,link=/tmp/ttyBase
