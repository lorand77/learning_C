# Factorial report: N = 12, plain int, loop vs recursion in C, Python and Node

The same loop and recursion as in `p19-recursion` and `p19b-recursion`, now
with N small enough that `N!` fits in a 32-bit `int`. That lets every language
use its plain, fastest integer type: no GMP, no `long`, no `BigInt`.

## What was measured

```
f = 1
for i in 2..N:  f = f * i                       # loop
fact(n) = n * fact(n - 1),  fact(1) = 1         # recursion
```

**N = 12.** 12! = 479,001,600 is the largest factorial that fits in a 32-bit
`int` (max 2,147,483,647); 13! = 6,227,020,800 does not.

Each run calls `factorial(12)` 1,000,000 times and the time is divided by the
number of calls. 6 runs per program, each one checks the result against
479,001,600.

| file | integer type | run as |
|---|---|---|
| `factorial_loop.c`, `factorial_rec.c` | `int` (32-bit signed) | `make && ./factorial_loop.bin` (`-O0`), `./factorial_loop_O2.bin` (`-O2`) |
| `factorial_loop.py`, `factorial_rec.py` | built-in `int` | `python3 factorial_loop.py` |
| `factorial_loop.js`, `factorial_rec.js` | plain `Number` | `node factorial_loop.js` |

Node no longer needs `BigInt`: 12! is far below 2^53, so a `Number` holds it
exactly, and the code uses plain `1`, `2`, `i++`.

The Makefile builds each C file twice: `foo.bin` without optimisation (gcc's
default `-O0`, as in p19 and p19b) and `foo_O2.bin` with `-O2`. In both, `n`
and the result are `volatile`, so the compiler cannot work out 12! ahead of
time or drop the calls.

Timers: `clock_gettime(CLOCK_MONOTONIC)` in C, `time.perf_counter()` in
Python, `performance.now()` in Node.

- Machine: AMD Ryzen 5 5600G, Linux 6.6 (WSL2), single-threaded
- gcc 15.2.0, CPython 3.14.4, Node.js 24.20.0 (V8 13.6)
- Date: 2026-10-06

## Results

Time per `factorial(12)` call:

| | run 1 | runs 2-6 | best | vs C `-O2` (same method) |
|---|---|---|---|---|
| C loop `-O2` | 3.1 ns | 3.0-3.3 ns | **3.0 ns** | 1x |
| C recursion `-O2` | 2.7 ns | 2.6-3.1 ns | **2.6 ns** | 1x |
| C loop `-O0` | 22.4 ns | 16.6-22.7 ns | **16.6 ns** | 5.5x |
| C recursion `-O0` | 20.7 ns | 19.9-20.5 ns | **19.9 ns** | 7.7x |
| Node loop | 7.8 ns | 5.0-9.8 ns | **5.0 ns** | 1.7x |
| Node recursion | 37.1 ns | 36.0-47.8 ns | **36.0 ns** | 14x |
| Python loop | 252.8 ns | 243.5-306.3 ns | **243.5 ns** | 81x |
| Python recursion | 367.5 ns | 361.3-384.0 ns | **361.3 ns** | 139x |

**Optimised C is the fastest, about 3 ns per call.** gcc puts the factorial
inline into the timing loop and keeps everything in registers. The 11
multiplications are still all there (checked in the assembly, `gcc -O2 -S`);
nothing is precomputed. 11 multiplications in a row should take longer than
3 ns, but each call doesn't depend on the previous one, so the CPU most
likely works on several calls at once.

**`-O2` makes the C code 5-8x faster.** At `-O0` gcc keeps every variable in
memory and reloads it each step.

**Node's loop is close to optimised C** (5.0 vs 3.0 ns) and 3x faster than
unoptimised C. V8's optimising compiler (TurboFan) turns the hot JS loop into
tight machine code that works on 32-bit integers in registers, with an
overflow check on each multiply.

To make sure V8 wasn't precomputing `factorial(12)` because N is a constant,
a test version picked N from an array at run time, `fn(ns[r & 1])`, so the
compiler can't know it. Node's loop still took 5.1 ns per call (3.9 ns with
the constant), so the speed is real.

**Python is 80-140x slower than optimised C** and about 50x slower than Node's
loop. CPython runs the function in its interpreter, and every `f * i` creates
a new int object.

**What recursion costs depends on the language:**

| | recursion vs loop (best runs) |
|---|---|
| C `-O2` | no calls left, about the same (2.6 vs 3.0 ns) |
| C `-O0` | about the same (the loop's runs spread over 16.6-22.7 ns) |
| Python | +48% slower |
| Node | **7x slower** (36.0 vs 5.0 ns) |

- **C `-O2`:** gcc turns the recursion into a plain count-down loop
  (`f *= n; n--`) with no function calls at all; the assembly has no `call`
  to `factorial_with_recursion`. So the recursive source ends up as fast as
  the loop, here even slightly faster.
- **C `-O0`:** a call is a few instructions, close to the cost of a loop
  step that also goes through memory.
- **Python:** each call creates a new frame, which costs more than one more
  loop step.
- **Node:** the loop becomes about 11 machine-code multiplies, while
  recursion still makes 12 real function calls, each with a stack frame. V8
  doesn't turn recursion into a loop the way gcc does. When the loop itself
  is this cheap, the calls are almost all the cost.

## Compared with p19b-recursion (N = 20, 64-bit)

N = 20 does 19 multiplications, N = 12 does 11, so the per-step column
divides the best time by that count (rough, since each call also has a fixed
cost). p19b only has C at `-O0`, so that's what is compared.

| | p19b, N = 20 per call | p19c, N = 12 per call | p19b per step | p19c per step |
|---|---|---|---|---|
| C loop `-O0` | 30.2 ns | 16.6 ns | 1.6 ns | 1.5 ns |
| C recursion `-O0` | 34.9 ns | 19.9 ns | 1.8 ns | 1.8 ns |
| Python loop | 414.2 ns | 243.5 ns | 21.8 ns | 22.1 ns |
| Python recursion | 667.3 ns | 361.3 ns | 35.1 ns | 32.8 ns |
| Node loop | 128.9 ns (`BigInt`) | 5.0 ns (`Number`) | 6.8 ns | **0.45 ns** |
| Node recursion | 208.2 ns (`BigInt`) | 36.0 ns (`Number`) | 11.0 ns | 3.3 ns |

- **C and Python cost the same per step** whether the numbers are 32 or 64
  bits. For C, `long` and `int` multiply equally fast. For Python, both 12!
  and 20! are small ints, and the interpreter is the cost, not the arithmetic.
- **Node gets 15x faster per step** going from `BigInt` to `Number`. A
  `BigInt` is an object on the heap, and every operation makes a new one. A
  `Number` that holds a small integer lives in a register, and V8 compiles
  `f * i` to a single multiply instruction.

## Notes

**Overflow.** `int` is signed, and signed overflow in C is undefined
behaviour, so N must stay at 12 or below with `int`. In Node, a result that
outgrows 32 bits silently becomes a floating-point double. The answer stays
exact up to 2^53 (18!) and is rounded after that, with no error.
