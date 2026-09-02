# Make a list of N numbers 1, 2, ..., N.
# Then add 7 to every element, and time only that loop.

import time

N = 10000000

# Fill the list with 1, 2, ..., N. This part is not timed.
# range(1, N + 1) counts 1, 2, ..., N, and list() turns it into a real list.
arr = list(range(1, N + 1))

# This is the loop we care about: add 7 to every element.
start = time.perf_counter()

for i in range(N):
    arr[i] = arr[i] + 7

seconds = time.perf_counter() - start

print(f"Python : add 7 to {N} elements in {seconds:.4f} s")
