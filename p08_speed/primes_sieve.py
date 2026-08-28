# Count the first N primes with a sieve of Eratosthenes.
#
# Two variants of the marking step:
#   python3 primes_sieve.py N        slice assignment (the marking runs in C)
#   python3 primes_sieve.py N loop   a plain Python for loop, same algorithm

import sys
import time
from itertools import compress, islice
from math import isqrt, log


def limit_for(n):
    # The nth prime is smaller than n * (ln n + ln ln n) for n >= 6.
    if n < 6:
        return 15
    return int(n * (log(n) + log(log(n)))) + 1


def sieve_slice(limit):
    flags = bytearray([1]) * (limit + 1)
    flags[0:2] = b"\x00\x00"

    for i in range(2, isqrt(limit) + 1):
        if flags[i]:
            # mark i*i, i*i+i, i*i+2i, ... in one C-level slice assignment
            flags[i * i :: i] = b"\x00" * ((limit - i * i) // i + 1)

    return flags


def sieve_loop(limit):
    flags = bytearray([1]) * (limit + 1)
    flags[0:2] = b"\x00\x00"

    for i in range(2, isqrt(limit) + 1):
        if flags[i]:
            for j in range(i * i, limit + 1, i):
                flags[j] = 0

    return flags


def main():
    n = int(sys.argv[1]) if len(sys.argv) > 1 else 100000
    mode = sys.argv[2] if len(sys.argv) > 2 else "slice"

    start = time.perf_counter()

    limit = limit_for(n)
    flags = sieve_loop(limit) if mode == "loop" else sieve_slice(limit)

    # walk the flags and stop at the nth prime, without building a list
    last = next(islice(compress(range(limit + 1), flags), n - 1, None))

    seconds = time.perf_counter() - start

    print(f"Python : prime #{n} = {last} in {seconds:.3f} s   ({mode})")


main()
