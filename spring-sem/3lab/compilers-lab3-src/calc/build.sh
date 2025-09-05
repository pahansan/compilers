#!/bin/sh

bison -o calc.c calc.y
gcc -Wall -o calc calc.c -lm
rm calc.c

