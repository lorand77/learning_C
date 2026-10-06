import time

# same N as the C version: 12! is the largest factorial that fits in a 32-bit int
N = 12
EXPECTED = 479001600
REPS = 1_000_000

def factorial(n):
    f = 1
    for i in range(2, n + 1):
        f = f * i
    return f


for run in range(6):
    start_time = time.perf_counter()
    for rep in range(REPS):
        x = factorial(N)
    end_time = time.perf_counter()
    assert x == EXPECTED, f"wrong result: {x}"
    total = end_time - start_time
    print(f"run {run + 1}: {total:.3f}s ({total / REPS * 1e9:.1f} ns per call)")
