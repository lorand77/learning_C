import time

# same N as the C version: 12! is the largest factorial that fits in a 32-bit int
N = 12
EXPECTED = 479001600
REPS = 1_000_000

def factorial_with_recursion(n):
    if n > 1:
        return n * factorial_with_recursion(n-1)
    else:
        return 1


for run in range(6):
    start_time = time.perf_counter()
    for rep in range(REPS):
        x = factorial_with_recursion(N)
    end_time = time.perf_counter()
    assert x == EXPECTED, f"wrong result: {x}"
    total = end_time - start_time
    print(f"run {run + 1}: {total:.3f}s ({total / REPS * 1e9:.1f} ns per call)")
