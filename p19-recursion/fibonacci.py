import time
import sys

N = 100000
# N = 40
sys.setrecursionlimit(1000000)
sys.set_int_max_str_digits(1000000)

def fibonacci(n):
    if n == 0:
        return 0
    f = [0, 1]
    for i in range(2, n + 1):
        f.append(f[-1] + f[-2])
    return f[-1]


def fibonacci_with_recursion(n):
    # print(n, flush = True)
    if n > 1:
        return fibonacci_with_recursion(n-1) + fibonacci_with_recursion(n - 2)
    elif n == 1:
        return 1
    else:
        return 0
    

def fibonacci_with_recursion_fast(n, cache = {0: 0, 1: 1}):
    # same recursion, but every result is remembered in cache,
    # so each n is computed only once instead of over and over
    if n in cache:
        return cache[n]
    cache[n] = fibonacci_with_recursion_fast(n - 1, cache) + fibonacci_with_recursion_fast(n - 2, cache)
    return cache[n]


start_time = time.time()
x = fibonacci(N)
end_time = time.time()
print(end_time - start_time)
# print(x)

# start_time = time.time()
# x = fibonacci_with_recursion(N)
# end_time = time.time()
# print(end_time - start_time)
# print(x)

start_time = time.time()
x = fibonacci_with_recursion_fast(N)
end_time = time.time()
print(end_time - start_time)
# print(x)
