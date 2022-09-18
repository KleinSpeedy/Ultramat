#!/bin/sh

# Flash Arduino Ino Filo to Arduino Uno on ThinkPad

arduino-cli upload -b arduino:avr:uno -p /dev/ttyACM0 -v