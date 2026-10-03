import time
import sys

N = 100000
sys.setrecursionlimit(N + 10)
sys.set_int_max_str_digits(1000000)

def factorial(n):
    assert isinstance(n, int) and n >= 0, "n must be a non-negative integer"
    f = 1
    for i in range(2, n + 1):
        f = f * i
    return f


def factorial_with_recursion(n):
    if n > 1:
        return n * factorial_with_recursion(n-1)
    else:
        return 1

def factorial_with_recursion_with_assert(n):
    assert isinstance(n, int) and n >= 0, "n must be a non-negative integer"
    return factorial_with_recursion(n)


start_time = time.time()
x = factorial(N)
end_time = time.time()
print(end_time - start_time)

start_time = time.time()
x = factorial(N)
end_time = time.time()
print(end_time - start_time)


start_time = time.time()
x2 = factorial_with_recursion(N)
end_time = time.time()
print(end_time - start_time)

start_time = time.time()
x2 = factorial_with_recursion(N)
end_time = time.time()
print(end_time - start_time)


start_time = time.time()
x3 = factorial_with_recursion_with_assert(N)
end_time = time.time()
print(end_time - start_time)


# print(x)
# print(len(str(x)))
# print(x == x2)
