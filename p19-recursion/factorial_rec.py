import time
import sys

N = 50000
sys.setrecursionlimit(N + 10)

def factorial_with_recursion(n):
    if n > 1:
        return n * factorial_with_recursion(n-1)
    else:
        return 1

def factorial_with_recursion_with_assert(n):
    assert isinstance(n, int) and n >= 0, "n must be a non-negative integer"
    return factorial_with_recursion(n)


for run in range(6):
    start_time = time.perf_counter()
    x = factorial_with_recursion_with_assert(N)
    end_time = time.perf_counter()
    print(f"run {run + 1}: {end_time - start_time:.3f}s")
