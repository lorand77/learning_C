# The 4 stages of building a C program

Source: `sum.c` — computes `z = x + y` with `x = 1000`, `y = 2000`.

`gcc -o sum.bin sum.c` does all four stages at once. Here they are one by one.

```
sum.c  --[1 preprocessor]-->  sum.i  --[2 compiler]-->  sum.s
       --[3 assembler]-->     sum.o  --[4 linker]-->    sum.bin
```

---

## 1. Preprocessing — `.c` to `.i`

```
gcc -E sum.c -o sum.i
```

Text-level work only, no C is understood yet:
- `#include <stdio.h>` is replaced by the whole content of stdio.h (568 lines out of 10)
- `#define Y_VALUE 2000` is applied — `int y = Y_VALUE;` becomes `int y = 2000;`
- comments are stripped
- `# 5 "sum.c"` line markers are added so later errors can point back to the real file/line

The tail of `sum.i` is our code, now macro-free:

```c
# 5 "sum.c"
int main(void) {
  int x = 1000;
  int y = 2000;
  int z = x + y;
  printf("x = %d, y = %d, z = x + y = %d\n", x, y, z);
  return 0;
}
```

And somewhere above it, dragged in from stdio.h, the declaration that makes `printf` legal to call:

```c
extern int printf (const char *__restrict __format, ...);
```

## 2. Compiling — `.i` to `.s`

```
gcc -S sum.i -o sum.s
```

Now the real compiler runs: parsing, type checking, warnings, code generation.
Output is human-readable x86-64 assembly (`sum.s`), still text.

```asm
	movl	$1000, -12(%rbp)     # x lives 12 bytes below the frame pointer
	movl	$2000, -8(%rbp)      # y
	movl	-12(%rbp), %edx      # load x
	movl	-8(%rbp), %eax       # load y
	addl	%edx, %eax           # <-- the actual x + y
	movl	%eax, -4(%rbp)       # store into z
	movl	-4(%rbp), %ecx       # z -> 4th printf arg
	movl	-8(%rbp), %edx       # y -> 3rd arg
	movl	-12(%rbp), %eax
	leaq	.LC0(%rip), %rdi     # format string -> 1st arg
	movl	%eax, %esi           # x -> 2nd arg
	movl	$0, %eax             # 0 vector regs used (varargs ABI rule)
	call	printf@PLT
	movl	$0, %eax             # return 0
```

Note: `x`, `y`, `z` are gone as names — they are just stack offsets now.
The `@PLT` on `printf` means "we don't know the address yet, someone else must fill it in".

(Compile with `-O2` and this collapses to `printf("...", 1000, 2000, 3000)` —
the compiler does the addition itself.)

## 3. Assembling — `.s` to `.o`

```
gcc -c sum.s -o sum.o
```

Assembly text becomes binary machine code in an ELF **relocatable object file**.
Not runnable: addresses are still placeholders.

```
$ nm sum.o
0000000000000000 T main      <- T = defined text symbol, we provide it
                 U printf    <- U = undefined, we need it from somewhere
```

```
$ objdump -d sum.o
  c:  c7 45 f4 e8 03 00 00   movl $0x3e8,-0xc(%rbp)    <- 0x3e8 = 1000
 13:  c7 45 f8 d0 07 00 00   movl $0x7d0,-0x8(%rbp)    <- 0x7d0 = 2000
 20:  01 d0                  add  %edx,%eax
 2e:  48 8d 3d 00 00 00 00   lea  0x0(%rip),%rdi       <- address = 0, TBD
 3c:  e8 00 00 00 00         call 41 <main+0x41>       <- target = 0, TBD
```

Those zeros are listed as jobs for the linker:

```
$ objdump -r sum.o
OFFSET            TYPE             VALUE
0000000000000031  R_X86_64_PC32    .rodata-0x4    <- patch in the format string addr
000000000000003d  R_X86_64_PLT32   printf-0x4     <- patch in the printf call target
```

## 4. Linking — `.o` to executable

```
gcc sum.o -o sum.bin
```

The linker (`ld`, invoked by gcc via `collect2`) does the rest:
- adds the C runtime startup objects `Scrt1.o`, `crti.o`, `crtbeginS.o` — these define
  `_start`, which sets up argc/argv and then calls `main`
- links `-lc` (libc, where `printf` lives) and `-lgcc`
- records `/lib64/ld-linux-x86-64.so.2` as the dynamic linker
- assigns real addresses and patches every relocation

Same instructions, now with the blanks filled in:

```
0000000000001149 <main>:
    1177:  48 8d 3d 8a 0e 00 00   lea  0xe8a(%rip),%rdi   # 2008 <- real string addr
    1185:  e8 c6 fe ff ff         call 1050 <printf@plt>  <- real call target
```

```
$ nm sum.bin | wc -l     -> 30 symbols (was 2)
$ readelf -h sum.bin     -> Entry point address: 0x1060   (_start, not main)
$ ldd sum.bin
	libc.so.6 => /usr/lib/x86_64-linux-gnu/libc.so.6
	/lib64/ld-linux-x86-64.so.2
```

`printf` itself is still *not* inside the binary — it is resolved at program start by
the dynamic linker. Use `gcc -static` to bake it in (binary jumps from ~16 KB to ~800 KB).

## Run it

```
$ ./sum.bin
x = 1000, y = 2000, z = x + y = 3000
```

---

## All commands together

```
gcc -E sum.c -o sum.i     # 1. preprocess
gcc -S sum.i -o sum.s     # 2. compile
gcc -c sum.s -o sum.o     # 3. assemble
gcc sum.o -o sum.bin      # 4. link
./sum.bin
```

Useful inspection commands:

```
nm sum.o                  # symbol table: T = defined, U = undefined
objdump -r sum.o          # relocation entries (linker to-do list)
objdump -d sum.o          # disassemble
readelf -h sum.bin        # ELF header, entry point
ldd sum.bin               # shared libraries needed at run time
gcc -save-temps -o sum.bin sum.c   # keep .i and .s from a normal build
```
