#!/bin/bash
# Compile both C benchmarks twice (optimised and not) and run everything.

gcc -std=c11 -O0 -o bench_append_O0.bin bench_append.c
gcc -std=c11 -O2 -o bench_append_O2.bin bench_append.c
gcc -std=c11 -O0 -o bench_append_seq_O0.bin bench_append_seq.c
gcc -std=c11 -O2 -o bench_append_seq_O2.bin bench_append_seq.c

echo "--- appending 7 every time ---"
echo -n "gcc -O0 -> "
./bench_append_O0.bin
echo -n "gcc -O2 -> "
./bench_append_O2.bin
echo -n "python3 -> "
python3 bench_append.py

echo "--- appending 1..N ---"
echo -n "gcc -O0 -> "
./bench_append_seq_O0.bin
echo -n "gcc -O2 -> "
./bench_append_seq_O2.bin
echo -n "python3 -> "
python3 bench_append_seq.py
