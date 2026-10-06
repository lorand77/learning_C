# Factorial report: small N, loop vs recursion in C, Python and Node

The same loop and recursion as in `p19-recursion`, but with N small enough
that `N!` fits in a 64-bit integer. Without big numbers, each step is cheap, so
what's left to measure is the cost of the loop or the function calls.

## What was measured

```
f = 1
for i in 2..N:  f = f * i                       # loop
fact(n) = n * fact(n - 1),  fact(1) = 1         # recursion
```

**N = 20.** 20! = 2,432,902,008,176,640,000 (about 2.4e18) is the largest
factorial that fits in 64 bits; 21! (about 5.1e19) does not.

One `factorial(20)` takes nanoseconds, so each run calls it 1,000,000 times
and the time is divided by the number of calls. 6 runs per program, each one
checks the result against the exact value of 20!.

| file | integer type | run as |
|---|---|---|
| `factorial_loop.c`, `factorial_rec.c` | `long` (64-bit signed on Linux, max about 9.2e18) | `make && ./factorial_loop.bin` |
| `factorial_loop.py`, `factorial_rec.py` | built-in `int` | `python3 factorial_loop.py` |
| `factorial_loop.js`, `factorial_rec.js` | built-in `BigInt` | `node factorial_loop.js` |

Node uses `BigInt` because a plain `Number` is a double and only holds
integers exactly up to 2^53, which is 18!, not 20!. The Node loop counts with
`i = i + 1n`, not `i++`, which is about 3x slower on a `BigInt` (see below).

In C, `n` and the result are `volatile`, so the compiler cannot work out 20!
ahead of time or drop the calls. The Makefile has no `-O` flag, so the C code
is **not optimised** (gcc's default `-O0`), same as in `p19-recursion`.

Recursion depth is only 20, so Python doesn't need `sys.setrecursionlimit`
and Node doesn't need the worker thread with a big stack.

Timers: `clock_gettime(CLOCK_MONOTONIC)` in C, `time.perf_counter()` in
Python, `performance.now()` in Node.

- Machine: AMD Ryzen 5 5600G, Linux 6.6 (WSL2), single-threaded
- gcc 15.2.0, CPython 3.14.4, Node.js 24.20.0 (V8 13.6)
- Date: 2026-10-06

## Results

Time per `factorial(20)` call:

| | run 1 | runs 2-6 | best | vs C (same method) |
|---|---|---|---|---|
| C loop | 30.2 ns | 36.6-43.8 ns | **30.2 ns** | 1x |
| C recursion | 35.3 ns | 34.9-35.5 ns | **34.9 ns** | 1x |
| Python loop | 414.2 ns | 415.6-458.3 ns | **414.2 ns** | 14x |
| Python recursion | 667.3 ns | 667.3-730.3 ns | **667.3 ns** | 19x |
| Node loop | 141.1 ns | 128.9-139.8 ns | **128.9 ns** | 4x |
| Node recursion | 261.1 ns | 208.2-244.3 ns | **208.2 ns** | 6x |

**C is 4-19x faster than Python and Node.** In `p19-recursion` the gap was
only about 2x, because almost all the time went into multiplying huge numbers,
which all three languages do with optimised library code. Here the numbers are
small and the time goes into the loop and the calls themselves, and that is
where C is far ahead, even without optimisation.

**Node is about 3x faster than Python.** V8 compiles the hot function to
machine code; CPython runs it in its interpreter.

**What recursion costs depends on the language:**

| | recursion vs loop (best runs) |
|---|---|
| C | about the same (the loop's run-to-run noise, 30-44 ns, is bigger than the difference) |
| Python | +61% slower |
| Node | +62% slower |

- **C:** a function call is a few instructions, close to the cost of a loop
  step.
- **Python and Node:** each call costs more than one more step of a loop.
  With 20 calls per factorial, that shows up clearly, and by almost exactly
  the same ratio in both.

## Node: `i++` on a BigInt is slow

The first version of `factorial_loop.js` used `i++` and took about 435 ns per
call, so Node's recursion looked twice as fast as its loop. Changing one thing
at a time showed that the `++` was the cause, not loop vs recursion:

| variant | per call (best of 6) |
|---|---|
| `for (let i = 2n; i <= n; i++)` | 430 ns |
| `for (let i = 2n; i <= n; i = i + 1n)` | **130 ns** |
| count down: `while (n > 1n) { f = f * n; n--; }` | 389 ns |
| count down: `while (n > 1n) { f = f * n; n = n - 1n; }` | **130 ns** |
| recursion | 210 ns |

Only `++` / `--` matter. Counting up or down and comparing against `n` or the
constant `20n` make no difference. The recursive version happened to use
`n - 1n`, so it never hit the slow path.

V8's profiler (`node --prof`) shows why. With `i++`, time goes into the
`Increment` builtin and the stub that calls into V8's C++ code, and about 70%
of all ticks are inside that C++ code. With `i + 1n`, almost no builtin or C++
time shows up: the addition of two small BigInts runs inside the optimised
machine code. It isn't only the optimising compiler: with
`--no-turbofan --no-maglev`, `i++` is still about 1.8x slower than `+ 1n`
(597 vs 336 ns).

## Compared with p19-recursion (N = 50000)

| | p19 (N = 50000, huge numbers) | p19b (N = 20, fits in 64 bits) |
|---|---|---|
| time goes into | multiplying an 86 KB number | loop steps and function calls |
| C vs Python / Node | about 2x | 4-19x |
| recursion cost in Python | +4% | +61% |
| recursion cost in Node | +22% | +62% |
| problems with memory | yes, big ones | none seen |

With huge numbers the overhead of a loop or a call is lost next to the
multiply. With small numbers the overhead *is* the work, which is why
recursion costs so much more here.

## Notes

**Not optimised.** With `-O2`, gcc would likely make the C numbers much
smaller, and might turn the recursion into a loop. That was not tried here.

**Overflow.** `long` is signed, and signed overflow in C is undefined
behaviour, so N must stay at 20 or below with `long`. Plain `int` (32 bits)
would only go up to 12!.
