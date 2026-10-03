import time

N = 100000

def factorial(n):
    assert isinstance(n, int) and n >= 0, "n must be a non-negative integer"
    f = 1
    for i in range(2, n + 1):
        f = f * i
    return f


for run in range(6):
    start_time = time.perf_counter()
    x = factorial(N)
    end_time = time.perf_counter()
    print(f"run {run + 1}: {end_time - start_time:.3f}s")
