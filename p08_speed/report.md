# Speed report: first N primes by trial division

C vs Node.js vs Python, same algorithm, same machine.

## What was measured

All three programs implement the identical algorithm: count up from 2, test each
candidate by trial division with every `d` where `d * d <= n`, stop when N primes
have been found, print the Nth prime.

| file | run as |
|---|---|
| `primes.c` | `gcc -O2 -o primes.bin primes.c && ./primes.bin N` |
| `primes.js` | `node primes.js N` |
| `primes.py` | `python3 primes.py N` |

Each program times *itself* (`clock()`, `process.hrtime`, `time.perf_counter`),
so the numbers below exclude process startup and C compile time. The Nth prime is
printed so nothing can be optimised away, and all three print the same value.

- Machine: AMD Ryzen 5 5600G, Linux 6.6 (WSL2), single-threaded
- gcc 15.2.0 `-O2`, Node v24.20.0, CPython 3.14.4
- Date: 2026-08-28

## Results

| N | Nth prime | C (-O2) | Node.js | Python |
|---|---|---|---|---|
| 100,000 | 1,299,709 | 0.139 s | 0.143 s | 3.873 s |
| 500,000 | 7,368,787 | 1.621 s | 1.705 s | 45.364 s |

Relative to C at N = 500,000:

| | C | Node.js | Python |
|---|---|---|---|
| times slower | 1.0x | 1.05x | 28.0x |

## What the numbers say

**Node is within ~5% of C here.** V8 sees a hot integer loop with monomorphic
types, JIT-compiles it to machine code, and from then on runs essentially the
same instruction sequence gcc emits. This is the best case for a JIT: no objects,
no strings, no allocation, so no garbage collector, and enough iterations that
compile time disappears into the noise.

**Python is ~28x slower.** CPython interprets bytecode; every `d * d` and
`n % d` dispatches through the eval loop and allocates a new `int` object. There
is no JIT in CPython 3.14 by default.

**`-O2` only bought ~8%.** The same C compiled `-O0` ran 1.760 s vs 1.621 s. The
loop is dominated by the hardware integer division instruction (`idiv`, tens of
cycles), which no amount of instruction scheduling can speed up. Optimisation
flags matter most when there is redundant work to remove; here there is very
little.

**Startup is not free, and it does not show in the table above.** Measured as
best-of-5 wall clock for a do-nothing run: C binary ~7 ms, `python3 -c ''`
~19 ms, `node -e ''` ~31 ms. At N = 1000 the whole computation takes ~3 ms and
total wall clock is C 9 ms / Python 30 ms / Node 55 ms, so Node is the *slowest*
of the three despite having the fastest loop. Which language "wins" depends on
how long the program runs.

## Compared with the Computer Language Benchmarks Game

<https://benchmarksgame-team.pages.debian.net/benchmarksgame/index.html>

The Benchmarks Game runs ten programs (fannkuch-redux, n-body, spectral-norm,
mandelbrot, fasta, k-nucleotide, reverse-complement, binary-trees, pidigits,
regex-redux) on a fixed quad-core i5-3330 under Ubuntu 24.04, measuring full
elapsed wall clock with BenchExec, best of 12 runs, caches cleared. Fastest
entry per language, from the 25.03 results:

| benchmark | C gcc | Node.js | Python 3 | Node/C | Py/C |
|---|---|---|---|---|---|
| fannkuch-redux | 2.14 | 11.07 | 311.18 | 5.2x | 145x |
| n-body | 2.10 | 8.55 | 372.41 | 4.1x | 177x |
| spectral-norm | 0.40 | 1.60 | 90.37 | 4.0x | 226x |
| mandelbrot | 1.29 | 4.05 | 182.94 | 3.1x | 142x |
| fasta | 0.79 | 2.56 | 27.95 | 3.2x | 35x |
| k-nucleotide | 3.58 | 15.99 | 46.55 | 4.5x | 13x |
| reverse-complement | 0.44 | 15.53 | 4.18 | 35.3x | 9.5x |
| binary-trees | 1.56 | 8.61 | 33.37 | 5.5x | 21x |
| pidigits | 0.74 | 1.15 | 1.35 | 1.6x | 1.8x |
| regex-redux | 0.81 | 4.35 | 1.41 | 5.4x | 1.7x |
| **geometric mean** | **1.0x** | **4.8x** | **27.4x** | | |

Our result: Node 1.05x, Python 28.0x.

**Python lands in the same place; Node does not.** Our Python/C ratio of 28x is
almost exactly their geometric mean of 27.4x — a coincidence in the sense that
their per-benchmark spread runs from 1.7x to 226x, but it does say our workload
is an unremarkable, representative interpreted-language slowdown. Our Node/C
ratio of 1.05x is far better than their 4.8x, and that gap needs explaining.

Four reasons the Node/C gap is wider in their results:

1. **Their C is hand-vectorised.** The fastest C entry is flagged `*` (possible
   hand-written vector instructions) in 5 of the 10 benchmarks. Compare fastest
   C against fastest *unflagged* C: spectral-norm 0.40 vs 1.43 s, n-body 2.10 vs
   4.98, fannkuch-redux 2.14 vs 7.28. Much of "C is 4.8x faster" is really "hand-
   written SIMD is faster than what a JIT will emit." Our `primes.c` is plain
   scalar code, and integer division does not vectorise anyway, so C gives up its
   biggest advantage.
2. **They measure the whole process.** Elapsed wall clock includes Node's ~31 ms
   startup and its JIT warmup; several of their programs run in 1–4 s, where that
   is a visible slice. Our timers start after the runtime is up.
3. **Their programs allocate.** binary-trees is a GC stress test; k-nucleotide
   builds hash tables; reverse-complement (Node 35x) is I/O and buffer churn.
   Our loop allocates nothing, which is where a JIT is strongest and where
   manual memory management stops paying off.
4. **Our benchmark is one narrow kernel.** A single hot integer loop is the
   friendliest possible shape for V8. Ten different programs sample far more of
   what real code does.

**And a caution their results make obvious:** on pidigits Python is only 1.8x
slower than C, and on regex-redux Python (1.41 s) beats Node (4.35 s) by 3x.
Both of those benchmarks spend nearly all their time inside GMP and PCRE — C
libraries called through bindings. The "speed of a language" is often the speed
of whatever native library it calls, not the speed of its own loops.

## Caveats

- One workload, one machine, no repeat runs or confidence intervals — treat
  the ratios as "roughly this order," not as measurements.
- Interpreted-language performance is version-sensitive; CPython 3.14 with a
  different build (or a JIT-enabled build) would move the Python column.
- Nothing here says anything about memory use, concurrency, startup-dominated
  workloads, or how quickly each version was written.

## Reproduce

```sh
./run.sh 500000
```
