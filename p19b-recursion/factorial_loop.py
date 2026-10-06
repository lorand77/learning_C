import time

# same N as the C version: 20! is the largest factorial that fits in 64 bits
N = 20
EXPECTED = 2432902008176640000
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
