#!/bin/bash
# Usage: ./run.sh [N]   (N = how many primes to find)

N=${1:-100000}

gcc -O2 -o primes.bin primes.c
gcc -O2 -o primes_sieve.bin primes_sieve.c -lm

echo "trial division, first $N primes:"
./primes.bin $N
node primes.js $N
python3 primes.py $N

echo
echo "sieve of Eratosthenes, first $N primes:"
./primes_sieve.bin $N
node primes_sieve.js $N
python3 primes_sieve.py $N
python3 primes_sieve.py $N loop
