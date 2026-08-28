# Count the first N primes by simple trial division.

import sys
import time


def is_prime(n):
    if n < 2:
        return False
    d = 2
    while d * d <= n:
        if n % d == 0:
            return False
        d += 1
    return True


def main():
    n = int(sys.argv[1]) if len(sys.argv) > 1 else 100000

    start = time.perf_counter()

    count = 0
    last = 0
    candidate = 1

    while count < n:
        candidate += 1
        if is_prime(candidate):
            count += 1
            last = candidate

    seconds = time.perf_counter() - start

    print(f"Python : prime #{n} = {last} in {seconds:.3f} s")


main()
