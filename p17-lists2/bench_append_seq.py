# Same benchmark as bench_append.py, but appending the numbers 1..N
# instead of the same 7 every time.

import time

N = 10_000_000

numbers = []
start = time.perf_counter()
for i in range(1, N + 1):
    numbers.append(i)
elapsed = time.perf_counter() - start

print(f"python: {len(numbers)} appends in {elapsed * 1000:7.1f} ms"
      f"  ({elapsed * 1e9 / N:6.1f} ns each)  last={numbers[-1]}")
