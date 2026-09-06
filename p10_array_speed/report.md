# Array speed report: adding 7 to ten million elements

C vs Node.js vs Python, same array, same loop, same machine.

## What was measured

All three programs make an array/list of `N = 10000000` elements holding
`1, 2, ..., N`, then run one loop over it:

```
arr[i] = arr[i] + 7
```

**Only that loop is timed** (`clock()` in C, `process.hrtime.bigint()` in Node,
`time.perf_counter()` in Python). Building the array beforehand is not included,
and neither is process startup or C compile time.

The C program is compiled twice from the same source, once unoptimised and once
optimised, to show how much the compiler alone is worth.

| file | run as |
|---|---|
| `array_add.c` | `gcc -O0 -o array_add_O0.bin array_add.c && ./array_add_O0.bin` |
| `array_add.c` | `gcc -O2 -o array_add_O2.bin array_add.c && ./array_add_O2.bin` |
| `array_add.js` | `node array_add.js` |
| `array_add.py` | `python3 array_add.py` |

Or just `./run.sh`, which does all four.

- Machine: AMD Ryzen 5 5600G (16 MB L3 cache), Linux 6.6 (WSL2), single-threaded
- gcc 15.2.0, Node.js 24.20.0 (V8 13.6), CPython 3.14.4
- Date: 2026-09-06

## Results

Median of 5 runs (7 for Node, see the warm-up note below):

| | time for 10,000,000 updates | per element | times slower than C `-O2` |
|---|---|---|---|
| C, `gcc -O2` | 0.0029 s | 0.29 ns | 1x |
| Node | 0.0092 s | 0.92 ns | ~3.2x |
| C, `gcc -O0` | 0.0105 s | 1.05 ns | ~3.6x |
| Python | 0.5764 s | 58 ns | **~199x** |

Raw output of one run:

```
gcc -O0 -> C      : add 7 to 10000000 elements in 0.0105 s
gcc -O2 -> C      : add 7 to 10000000 elements in 0.0029 s
node    -> Node   : add 7 to 10000000 elements in 0.0093 s
python3 -> Python : add 7 to 10000000 elements in 0.6755 s
```

## What the numbers say

**Node lands next to unoptimised C, not next to Python.** That is the surprise
of this benchmark if you expect "compiled fast, scripting slow". JavaScript is
a dynamic language with no type declarations, exactly like Python — but V8
compiles this loop to machine code that loads a value, adds 7 and stores it
back, and it does that ~63x faster than CPython does.

**The reason is layout, not just JIT.** In C, `arr[i] = arr[i] + 7` is "load 4
bytes, add 7, store 4 bytes" over one flat 40 MB block. In CPython the same line
is a sequence of interpreted bytecode: look up `arr`, look up `i`, index the
list — which gives back a *pointer to an int object* — allocate a **new** int
object holding the old value plus 7, and store that pointer back. A Python list
of ten million ints is ten million pointers to ten million separate heap
objects. V8 does neither of those things: when every element of an array is a
small integer, it stores them inline as a flat block of tagged values, so
`arr[i] + 7` is arithmetic on a machine word, not an allocation.

**The compiler is worth ~3.6x.** `-O0` emits the loop literally: `i` lives in
memory and is reloaded and rewritten every iteration, one element at a time.
`-O2` keeps `i` in a register and *vectorises* the loop, adding 7 to eight
elements at once with SIMD instructions. That is also the whole of C's lead over
Node here — unoptimised C is *slower* than Node.

**Python costs the same ~58 ns per element no matter the size**, because that
time is interpreter work, not memory work. C does not: at `N = 10000000` the
40 MB array no longer fits in this CPU's 16 MB L3 cache, so the optimised loop
has to stream 40 MB in from RAM and write 40 MB back. That is why C `-O2` here
costs 0.29 ns per element rather than the ~0.2 ns it manages on an array small
enough to stay in cache. **C is now limited by memory bandwidth, not by the
CPU** — which is why making the arithmetic faster stops paying off as much as
you would expect.

### A note on timing Node

The **first** run of the Node loop in a fresh process is often ~3x slower than
the ones after it — 0.031 s against a steady 0.009 s in one batch here. V8 starts
by interpreting the loop and only compiles it to optimised machine code once it
has seen it run hot. There is nothing equivalent in C (compiled ahead of time)
or in CPython (never compiled at all), so a single Node run is not a measurement.
The medians above come from repeated runs with the outliers visible.

## Memory

Speed is not the only difference. Peak resident memory of each program:

| | peak memory | of which is the array |
|---|---|---|
| C (`-O2`) | 39.6 MB | 40 MB |
| Node | 129.1 MB | ~80 MB |
| Python | **392.1 MB** | ~343 MB |

Subtract each runtime's empty baseline — 44.2 MB for `node -e 0`, 11.4 MB for
`python3 -c pass` — and the story is just the width of one element:

| | bytes per element |
|---|---|
| C `int` | 4 |
| JS array of small integers | 8 (a tagged value, still inline) |
| Python `int` in a `list` | 8 for the pointer + 28 for the object |

That last row is the same fact as the timing, seen from the other side. An `int`
in CPython carries a reference count, a type pointer and a length field before it
gets to the actual digits. And because those objects are immutable, `arr[i] + 7`
cannot overwrite 4 bytes in place; it allocates a brand-new int object every
single time. Ten times the memory for the same ten million numbers.

(Values from -5 to 256 are shared singletons in CPython, so a handful of the
elements cost nothing extra — but that is only ~250 out of ten million.)

## Things to try

- Change `N` and watch where the C time stops scaling linearly. Around 4,000,000
  elements the array outgrows the 16 MB L3 cache and the per-element cost jumps,
  while Python's per-element cost barely moves.
- Try `-O1` and `-O3` as well, and see where the big jump happens.
- Run the Node version with `--jitless` and watch it fall from 0.009 s to
  ~0.19 s. That splits Node's win in two: the JIT is worth ~21x, and the flat
  array layout is worth the remaining ~3x that interpreted JavaScript still has
  over Python.
- In Python, replace the loop with a list comprehension `arr = [x + 7 for x in arr]`,
  or use `numpy`. The comprehension skips the repeated indexing bytecode; numpy
  is far faster still — and uses ~40 MB instead of ~390 MB — because it stores a
  flat block of machine ints and does the add in C, exactly like `array_add.c`.

See also [`../p08_speed/report.md`](../p08_speed/report.md), which compares C,
Node.js and Python on prime-finding — a compute-bound loop rather than this
memory-bound one.
