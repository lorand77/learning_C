# Append speed report: ten million appends

Python list vs the growing C list from `list5_grow.c`, same machine.

## What was measured

Both programs start with an empty list and append `N = 10000000` values,
letting the list grow as needed. Two variants of each: one appends the same
value `7` every time, the other appends the running numbers `1..N`.

**Only the append loop is timed** (`clock_gettime(CLOCK_MONOTONIC)` in C,
`time.perf_counter()` in Python). Process startup and the C compile are not
included.

The C list uses the same rule as `list5_grow.c`: start at 16, and when full
`realloc` to `cap + cap / 4` (a factor of 1.25). It never shrinks.

| file | run as |
|---|---|
| `bench_append.c` | `gcc -std=c11 -O0 -o bench_append_O0.bin bench_append.c && ./bench_append_O0.bin` |
| `bench_append.c` | `gcc -std=c11 -O2 -o bench_append_O2.bin bench_append.c && ./bench_append_O2.bin` |
| `bench_append.py` | `python3 bench_append.py` |
| `bench_append_seq.c` | `gcc -std=c11 -O2 -o bench_append_seq_O2.bin bench_append_seq.c && ./bench_append_seq_O2.bin` |
| `bench_append_seq.py` | `python3 bench_append_seq.py` |

Or just `./run.sh`, which builds and runs all six.

- Python 3.14.4, gcc 15.2.0, Linux 6.6 (WSL2), single-threaded
- 5 runs each

## Results: appending `7` every time

| | per append | total | vs Python |
|---|---|---|---|
| C, `gcc -O2` | 4.1 ns | 41 ms | **7.8x faster** |
| C, `gcc -O0` | 5.5 ns | 55 ms | 5.8x faster |
| Python | 31 ns | 310 ms | 1x |

Spread over 5 runs: Python 308-329 ms, C -O2 41.1-41.9 ms, C -O0 55-80 ms.
The -O0 numbers are the noisiest; the best of them is the fair one to quote.

Per-append cost is the same as it was at one million appends, so nothing
here degrades with size -- appending stays amortised O(1) in both languages.

## Results: appending `1..N`

Same lists, same growth rule, only the value changes: `append(i)` for
`i = 1..N` instead of `append(7)`.

| | per append | total | vs Python |
|---|---|---|---|
| C, `gcc -O2` | 3.8 ns | 38 ms | **15x faster** |
| C, `gcc -O0` | 5.3 ns | 53 ms | 10.6x faster |
| Python | 57 ns | 566 ms | 1x |

Spread over 5 runs: Python 566-742 ms, C -O2 37.6-39.7 ms, C -O0 53.3-56.8 ms.

Side by side with the constant-`7` runs, re-measured in the same session so
the comparison is fair:

| | append `7` | append `1..N` | change |
|---|---|---|---|
| C, `gcc -O2` | 39.5 ms | 37.6 ms | none (inside the noise) |
| C, `gcc -O0` | 52.7 ms | 53.3 ms | none (inside the noise) |
| Python | 299 ms | 566 ms | **1.9x slower** |
| Python peak memory | 85 MB | 392 MB | 4.6x more |

**C does not care what the number is.** The loop stores a 4-byte `int` into
an array either way, so the value being a counter instead of a constant
changes nothing measurable.

**Python nearly doubles**, and the reason is object allocation. A Python list
holds pointers, so every element has to be an `int` object somewhere. When
appending `7`, that is always the *same* object: CPython caches the small
integers -5..256, so the loop just bumps a reference count on one cache-hot
object. When appending `1..N`, all but the first 256 values need a fresh
28-byte `int` object that the list then keeps alive -- ten million
allocations that can never be recycled, touching 280 MB of new memory on top
of the 80 MB of pointers. That is the 392 MB above, against 40 MB of raw
`int` in the C version.

So the honest way to state the gap: C is about 8x faster than Python at
appending a cached small integer, and about 15x faster at appending numbers
large enough to need real objects. The list machinery is not what differs --
the cost of having values at all is.

## Notes

**Python's 31 ns is not the array write.** Storing into the array is a few
nanoseconds at most. The rest is interpreter work that C does not do:
fetching the `append` attribute, building and dispatching a method call, and
bumping the reference count on the shared `7` object. A Python list stores
8-byte pointers to objects, not the numbers themselves -- 80 MB here, against
40 MB of raw `int` in C.

**Optimisation is worth little here** (-O0 to -O2 is only 1.3x). The loop is
dominated by memory traffic and a `realloc` call the compiler cannot remove,
not by arithmetic it can improve.

**The growth factor barely matters.** Rebuilt with `cap * 2` instead of
`cap + cap / 4`, the same benchmark ran in 40.4 ms against 43.4 ms -- about
7%, close to run-to-run noise. Instrumenting the C version explains why: over
ten million appends the list grew 61 times, but `realloc` kept the block where
it was in 52 of those, and actually copied the data only 9 times. glibc can
extend a large heap block in place, so most of the extra reallocations a 1.25
factor causes cost almost nothing.

The tradeoff 1.25 does buy is memory: at ten million elements a doubling list
can sit on up to twice what it needs, while 1.25 wastes at most a quarter.
That is the same reasoning behind CPython's own growth rule,
`newsize + (newsize >> 3) + 6`, which is a factor of about 1.125.
