# Append the number 7 one million times to a plain Python list.

import time

N = 10_000_000

numbers = []
start = time.perf_counter()
for i in range(N):
    numbers.append(7)
elapsed = time.perf_counter() - start

print(f"python: {len(numbers)} appends in {elapsed * 1000:7.1f} ms"
      f"  ({elapsed * 1e9 / N:6.1f} ns each)")
