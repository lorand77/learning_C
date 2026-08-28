#!/bin/bash
# Usage: ./run.sh [N]   (N = how many primes to find)

N=${1:-100000}

gcc -O2 -o primes.bin primes.c

echo "first $N primes:"
./primes.bin $N
node primes.js $N
python3 primes.py $N
