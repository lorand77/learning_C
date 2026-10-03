# Factorial report: loop vs recursion in C, Python and Node

Compute `N!` with a loop and with recursion, in three languages, and time it.
The real story turned out to be memory, not recursion.

## What was measured

Each program computes `N!` exactly (big integers) and times only the
computation, repeated several times in one process:

```
f = 1
for i in 2..N:  f = f * i                       # loop
fact(n) = n * fact(n - 1),  fact(1) = 1         # recursion
```

Like `x = factorial(N)` in Python, every program keeps the previous result
alive while it computes the next one.

| file | big integers | run as |
|---|---|---|
| `factorial_loop.c`, `factorial_rec.c` | GMP `mpz_t`, `mpz_mul_ui(f, f, i)` in place | `make && ./factorial_loop.bin` |
| `factorial_loop.py`, `factorial_rec.py` | built-in `int` (immutable) | `python3 factorial_loop.py` |
| `factorial_loop.js`, `factorial_rec.js` | built-in `BigInt` (immutable) | `node factorial_loop.js` |
| `run_loop_python.sh`, `run_rec_python.sh` | Python with tuned glibc malloc (see below) | `./run_loop_python.sh` |

Timers: `clock_gettime(CLOCK_MONOTONIC)` in C, `time.perf_counter()` in
Python, `performance.now()` in Node.

Recursion depth needs room: Python raises its limit with
`sys.setrecursionlimit(N + 10)`, and Node runs the recursive version in a
worker thread with a 256 MB stack, because 100,000 nested calls do not fit in
the main thread's stack. C manages on the default 8 MB stack.

- Machine: AMD Ryzen 5 5600G, Linux 6.6 (WSL2), single-threaded
- gcc 15.2.0 with GMP 6.3.0, CPython 3.14.4, Node.js 24.20.0 (V8 13.6)
- Date: 2026-10-03

## Results at N = 50000 (the clean benchmark)

The programs are currently set to this: `N = 50000`, 6 runs each.
50000! has 213,237 digits, about 86 KB.

| | run 1 | runs 2-6 | best | vs C |
|---|---|---|---|---|
| C loop | 0.156 s | 0.154-0.158 s | **0.154 s** | 1x |
| C recursion | 0.159 s | 0.154-0.157 s | **0.154 s** | 1x |
| Python loop | 0.294 s | 0.292-0.294 s | **0.292 s** | 1.9x |
| Python recursion | 0.332 s | 0.304-0.336 s | **0.304 s** | 2.0x |
| Node loop | 0.388 s | 0.333-0.380 s | **0.333 s** | 2.2x |
| Node recursion | 0.852 s | 0.405-0.510 s | **0.405 s** | 2.6x |

Python with the tuned malloc gave the same times (best 0.291 s loop,
0.302 s recursion), so at this size the allocator does not matter.

**C is about 2x faster than Python and Node.** All three use the same simple
method here: a big number times a small one, piece by piece. GMP works in
64-bit pieces with hand-written assembly, Python in 30-bit pieces, so Python
has about twice as many pieces to go through. Python and Node also build a new
86 KB number every step and copy into it, while GMP grows one buffer in place.

**What recursion costs depends on the language:**

| | recursion vs loop | why |
|---|---|---|
| C | 0% | a function call is a few instructions, tiny next to an 86 KB multiply |
| Python | +4% | 50,000 nested frames need several MB of memory (about 1,800 page faults per run) |
| Node | +22%, slow first run | the big worker stack has to be faulted in, and V8 needs a run or two to optimise the deep calls |

## Results at N = 100000 (where things went wrong)

100000! has 456,574 digits, about 190 KB. Same programs, 10 runs each.

| | run 1 | later runs | page faults |
|---|---|---|---|
| C loop | 0.68 s | 0.68-0.75 s, flat | 213 in all 10 runs |
| C recursion | 0.74 s | 0.68-0.70 s, flat | 993 in all 10 runs |
| Python loop | 2.20 s | alternating ~1.3 s / ~1.9 s | up to 286,000 per run |
| Python recursion | 2.29 s | alternating ~1.3 s / ~1.9 s | up to 286,000 per run |
| Python loop, tuned malloc | 1.31 s | 1.26-1.34 s, flat | ~150 |
| Python recursion, tuned malloc | 1.30 s | 1.29-1.35 s, flat | ~150 |
| Node loop | 7.48 s | 5.98-7.24 s, noisy | about 1.1 million per run |
| Node recursion | 8.09 s | 6.20-6.65 s, noisy | about 1.1 million per run |

Doubling N should cost about 4x (twice the multiplications, each on a number
twice as long). C does exactly that: 0.154 s to 0.68 s. Python's best run
does too: 0.292 s to 1.26 s. Everything above that, the slow first run, the
odd-even pattern and Node's 6 seconds, is time spent getting memory from the
operating system. Each page fault (the kernel handing out and zeroing a fresh
4 KB page) costs about 3 µs on this machine, and the time follows the fault
count closely in every run.

All three problems start at the same size: **objects bigger than 128 KB.**
100000! passes that at around `i = 70000`; 50000! never reaches it.

### Python: the slow first run

glibc's `malloc` hands blocks of 128 KB or more straight to `mmap`, which gets
fresh pages from the kernel, and gives them back with `munmap` when freed.
After such a free, glibc raises its 128 KB cutoff to that block's size, but the
next int is a bit bigger again, so during the first run nearly every large
allocation lands just over the cutoff and gets fresh pages. One run touched
about 1.1 GB of fresh memory to produce a 190 KB result: 2.2 s instead of
1.3 s. By the second run the cutoff has caught up and blocks come from the
normal, reused heap.

### Python: the odd-even pattern

`x = factorial(N)` keeps the old `x` alive until the new one is finished, so
the new result cannot reuse the old one's memory. The results alternate
between two spots in the heap:

| run | old `x` sits | new `x` lands | page faults | time |
|---|---|---|---|---|
| 2, 4, 6, ... | low in the heap | top of the heap | 86,420 | slow |
| 3, 5, 7, ... | top of the heap | low in the heap | 18 | fast |

The heap can only grow or shrink at its top end, and `malloc` gives memory back
to the OS once more than about 2 x 190 KB is free at the top. When the old
result sits at the top, it works like a lid: all the temporary ints are made
and freed in the gap below it, and nothing is given back. When it sits low,
the temporaries grow at the top, every free pushes the free space at the top
past the limit, the memory goes back to the OS, and the next int has to fault
it in again.

Setting the old result to `None` before each run removed the pattern, but
every run then took a steady 1.8 s, slower than the fast runs.

### The fix for Python: two environment variables

```
MALLOC_TRIM_THRESHOLD_=1073741824 MALLOC_MMAP_THRESHOLD_=1073741824 python3 factorial_loop.py
```

The first stops `malloc` from giving memory back, the second stops it from
using `mmap` for large blocks. The heap then grows once to a few hundred KB and
is reused for the rest of the program: about 150 page faults instead of
286,000, and every run, including the first, takes about 1.27 s. That is what
`run_loop_python.sh` and `run_rec_python.sh` do.

Both settings are needed. Setting only `MALLOC_MMAP_THRESHOLD_` (to 64 MB)
made every run slow (about 2.5 s, 368,000 faults), because setting it also
switches off glibc's automatic adjustment and leaves the 128 KB trim limit in
place, so the heap kept being shrunk back to the OS.

### Node: 5x slower than Python, every run

Below 128 KB, Node is as fast as Python:

| N | Python | Node |
|---|---|---|
| 20000 (31 KB result) | 0.045 s | 0.064 s |
| 60000 (106 KB result) | 0.557 s | 0.538 s |

Past 128 KB the cost jumps:

| N | result | Node time | page faults |
|---|---|---|---|
| 60000 | 106 KB | 0.55 s | 12k |
| 68000 | 121 KB | 0.72 s | 20k |
| 72000 | 129 KB | 0.90 s | 40k |
| 80000 | 145 KB | 2.15 s | 318k |

Going from 72000 to 80000 costs seven times more than going from 60000 to
68000, for the same 8,000 multiplications.

V8 packs new objects into 256 KB pages that it reuses. An object bigger than
half a page gets its own block of memory from the OS. The garbage collector
frees the dead ones every ~40 ms and gives their memory straight back
(`--trace-gc` shows `pooled: 0 MB`), so every new BigInt over 128 KB gets
fresh pages: about 1.1 million faults per run.

Garbage collection itself is not the cost. The trace for one 100000! run shows
727 small collections and 2 full ones, less than 0.5% of the time. Unlike
glibc, V8 never adjusts, so Node does not warm up, and none of its options
keep or pool large blocks.

### C: no problem

GMP's `mpz_mul_ui(f, f, i)` writes the result back into `f`, growing the same
buffer. There is no new 190 KB object per step, so memory is requested a few
hundred times in total, not hundreds of thousands. C was flat from run 1 at
both sizes.

## What it all says

**Loop vs recursion barely matters.** It costs nothing in C, a few percent in
Python and about 20% in Node, where the big stack has its own costs.

**C is about 2x faster at the arithmetic** than Python and Node.

**Memory decides the rest.** The big surprises at N = 100000 (the 2.2 s first
run, the 1.3 s / 1.9 s alternation, Node being 5x slower) all came from how
each runtime handles objects over 128 KB. Immutable big integers make a new
object every step; how expensive that is depends entirely on whether the
allocator reuses memory or gets it fresh from the kernel.

**For fair timings:** time only the work, repeat it in one process, compare
best runs, and watch the page-fault count (`resource.getrusage` in Python).
If one run is much slower than the next, the allocator is a likely cause.

## Notes

**Avoiding the huge objects.** Building fewer huge BigInts would avoid
the problem: multiply a few hundred consecutive `i` values together as small
numbers, then multiply that into `f`. Multiplying in a balanced tree is faster
still, and is how GMP's `mpz_fac_ui` and Python's `math.factorial` do it. These
programs keep the simple one-at-a-time loop on purpose, so the three languages
do the same work.

**The archived first attempt** (`archive/factorial.py`) timed the loop and the
recursion one after the other in one process. Recursion came out faster, but
only because it ran second and got the warmed-up allocator. In separate
processes the loop is slightly faster.
