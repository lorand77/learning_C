#!/bin/bash
# Compile the C program twice (optimised and not) and run everything.

gcc -O2 -o array_add_O2.bin array_add.c
gcc -O0 -o array_add_O0.bin array_add.c

echo -n "gcc -O0 -> "
./array_add_O0.bin
echo -n "gcc -O2 -> "
./array_add_O2.bin
echo -n "python3 -> "
python3 array_add.py
