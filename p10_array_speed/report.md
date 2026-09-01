# Array speed report: adding 7 to ten million elements

C vs Python, same array, same loop, same machine.

## What was measured

Both programs make an array/list of `N = 10000000` elements holding `1, 2, ..., N`,
then run one loop over it:

```
arr[i] = arr[i] + 7
```

**Only that loop is timed** (`clock()` in C, `time.perf_counter()` in Python).
Building the array beforehand is not included, and neither is process startup or
C compile time.

The C program is compiled twice from the same source, once unoptimised and once
optimised, to show how much the compiler alone is worth.

| file | run as |
|---|---|
| `array_add.c` | `gcc -O0 -o array_add_O0.bin array_add.c && ./array_add_O0.bin` |
| `array_add.c` | `gcc -O2 -o array_add_O2.bin array_add.c && ./array_add_O2.bin` |
| `array_add.py` | `python3 array_add.py` |

Or just `./run.sh`, which does all three.

- Machine: AMD Ryzen 5 5600G (16 MB L3 cache), Linux 6.6 (WSL2), single-threaded
- gcc 15.2.0, CPython 3.14.4
- Date: 2026-09-01

## Results

Median of 3 runs:

| | time for 10,000,000 updates | per element | times slower than C `-O2` |
|---|---|---|---|
| C, `gcc -O2` | 0.0032 s | 0.32 ns | 1x |
| C, `gcc -O0` | 0.0109 s | 1.09 ns | ~3.4x |
| Python | 0.6244 s | 62 ns | **~195x** |

Raw output of one run:

```
gcc -O0 -> C      : add 7 to 10000000 elements in 0.0109 s
gcc -O2 -> C      : add 7 to 10000000 elements in 0.0032 s
python3 -> Python : add 7 to 10000000 elements in 0.6244 s
```

## What the numbers say

**C is ~195x faster than Python on this loop.** In C, `arr[i] = arr[i] + 7` is
"load 4 bytes, add 7, store 4 bytes". In CPython the same line is a whole
sequence of interpreted bytecode: look up `arr`, look up `i`, index the list —
which gives back a *pointer to an int object* — allocate a **new** int object
holding the old value plus 7, and store that pointer back. A C `int` array of ten
million elements is one flat 40 MB block of numbers; a Python list of ten million
ints is ten million pointers to ten million separate heap objects.

**Python costs the same ~62 ns per element no matter the size**, because that
time is interpreter work, not memory work. C does not: at `N = 10000000` the
40 MB array no longer fits in this CPU's 16 MB L3 cache, so the optimised loop
has to stream all 40 MB in from RAM and write 40 MB back. That is why C `-O2`
here costs 0.32 ns per element rather than the 0.2 ns it manages on an array
small enough to stay in cache. **C is now limited by memory bandwidth, not by the
CPU.**

**The compiler is worth ~3.4x.** `-O0` emits the loop literally: `i` lives in
memory and is reloaded and rewritten every iteration, one element at a time.
`-O2` keeps `i` in a register and *vectorises* the loop, adding 7 to eight
elements at once with SIMD instructions. That gap is smaller than it would be on
a cache-sized array — once the bottleneck is RAM, making the arithmetic faster
stops helping much. Even so, unoptimised C is still ~57x faster than Python, so
the language gap is mostly the interpreter, not the optimiser.

## Memory

Speed is not the only difference. Peak resident memory of each program, measured
with `getrusage(RUSAGE_SELF)`:

| | peak memory |
|---|---|
| C (`-O2`) | 39.4 MB |
| Python | **392.0 MB** |

Where Python's 392 MB goes:

| | size |
|---|---|
| empty interpreter baseline | 9.0 MB |
| the list object (10,000,000 pointers, 8 bytes each, plus spare slots) | 76.3 MB |
| 10,000,000 `int` objects, 28 bytes each | 267.0 MB |
| allocator overhead and temporaries | ~40 MB |

This is the same fact as the timing, seen from the other side. C stores ten
million 4-byte values in one flat 40 MB block. Python stores ten million
*pointers* to ten million separate 28-byte heap objects — an `int` in CPython
carries a reference count, a type pointer and a length field before it gets to
the actual digits. And because those objects are immutable, `arr[i] + 7` cannot
overwrite 4 bytes in place; it allocates a brand-new int object every single
time. Ten times the memory for the same ten million numbers.

(Values from -5 to 256 are shared singletons in CPython, so a handful of the
elements cost nothing extra — but that is only ~250 out of ten million.)

## Things to try

- Change `N` and watch where the C time stops scaling linearly. Around 4,000,000
  elements the array outgrows the 16 MB L3 cache and the per-element cost jumps,
  while Python's per-element cost barely moves.
- Try `-O1` and `-O3` as well, and see where the big jump happens.
- In Python, replace the loop with a list comprehension `arr = [x + 7 for x in arr]`,
  or use `numpy`. The comprehension skips the repeated indexing bytecode; numpy
  is far faster still — and uses ~40 MB instead of ~390 MB — because it stores a
  flat block of machine ints and does the add in C, exactly like `array_add.c`.

See also [`../p08_speed/report.md`](../p08_speed/report.md), which compares C,
Node.js and Python on prime-finding — a compute-bound loop rather than this
memory-bound one.
