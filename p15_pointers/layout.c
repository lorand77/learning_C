/*
 * layout.c -- where do C variables actually live?
 *
 * Creates one variable in every region of the process address space and
 * prints where it landed.  Run it twice: every address changes, because
 * Linux randomizes the layout on each exec (ASLR).  What does NOT change
 * is the ordering and the leading digits of each region.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

/* ---- globals: fixed addresses, decided at compile time --------------- */

int g_initialized = 42;          /* .data   -- nonzero init, stored in file */
int g_zero;                      /* .bss    -- zero init, costs 0 file bytes */
char g_buffer[8192];             /* .bss    -- 8 KB of zeros, also free on disk */
const int g_const = 7;           /* .rodata -- const, write to it and you crash */
const char *g_literal = "hello"; /* pointer in .data, the "hello" in .rodata  */

/* ---- a table so we can sort everything by address at the end --------- */

#define MAX_ITEMS 40

struct item {
  uintptr_t   addr;
  const char *region;
  const char *name;
  const char *note;
};

static struct item items[MAX_ITEMS];
static size_t      n_items;

static void record(uintptr_t addr, const char *region,
                   const char *name, const char *note) {
  if (n_items < MAX_ITEMS) {
    items[n_items].addr   = addr;
    items[n_items].region = region;
    items[n_items].name   = name;
    items[n_items].note   = note;
    n_items++;
  }
  printf("  %-8s %-18s %#18" PRIxPTR "  %s\n", region, name, addr, note);
}

/* Taking the address of a function needs a detour: ISO C does not promise
 * a function pointer fits in a void *, so we go through uintptr_t. */
#define ADDR(x) ((uintptr_t)(x))

/* ---- functions live in .text ----------------------------------------- */

static void a_function(void) { }

/* Each call gets its own frame, stacked below the caller's. */
static void deeper(int level) {
  int local = level;
  static const char *names[] = { "", "local @ depth 1",
                                     "local @ depth 2",
                                     "local @ depth 3" };
  record(ADDR(&local), "stack", names[level],
         "one frame further down -- note the address DROPS");
  if (level < 3) deeper(level + 1);
}

static int by_addr(const void *a, const void *b) {
  const struct item *x = a;
  const struct item *y = b;
  if (x->addr < y->addr) return -1;
  if (x->addr > y->addr) return  1;
  return 0;
}

int main(int argc, char **argv) {
  /* ---- stack: born here, dead when main returns ---------------------- */
  int   local      = 1;
  char  local_arr[64];
  int  *ptr_to_local = &local;

  /* ---- static locals: scoped like locals, stored like globals -------- */
  static int s_initialized = 99;   /* .data */
  static int s_zero;               /* .bss  */

  /* ---- heap: alive until you free() it ------------------------------- */
  void *small = malloc(64);            /* small -> brk heap        */
  void *big   = malloc(16 * 1024 * 1024); /* large -> its own mmap */
  const char *msg = "on the heap";
  char *copy = malloc(strlen(msg) + 1);   /* +1 for the '\0' */

  if (!small || !big || !copy) { perror("malloc"); return 1; }

  strcpy(copy, msg);
  memset(local_arr, 0, sizeof local_arr);

  puts("region   what                          address  explanation");
  puts("-------- ------------------ -------------------  -----------");

  record(ADDR(a_function), ".text", "a_function()",
         "machine code; r-x, executable but NOT writable");
  record(ADDR(main), ".text", "main()",
         "also code -- functions are not variables, they have no storage");

  record(ADDR(g_literal), ".rodata", "\"hello\" literal",
         "string literals are read-only; modifying one is undefined behaviour");
  record(ADDR(&g_const), ".rodata", "g_const",
         "const global; the page itself is mapped read-only");

  record(ADDR(&g_initialized), ".data", "g_initialized",
         "nonzero initializer, so the value is stored inside the binary");
  record(ADDR(&s_initialized), ".data", "s_initialized",
         "static local: visible only in main(), but lives for the whole run");

  record(ADDR(&g_zero), ".bss", "g_zero",
         "zero-initialized, so the file stores only a size, not the bytes");
  record(ADDR(&s_zero), ".bss", "s_zero",
         "static local, also zeroed by the kernel before main() starts");
  record(ADDR(g_buffer), ".bss", "g_buffer[8192]",
         "8 KB of zeros that add nothing to the on-disk binary size");

  record(ADDR(small), "heap", "malloc(64)",
         "small request: carved out of the brk heap, just above the globals");
  record(ADDR(copy), "heap", "copy of a string",
         "the literal is in .rodata; this writable copy is on the heap");
  record(ADDR(big), "mmap", "malloc(16 MB)",
         "over ~128 KB glibc skips the heap and mmaps a fresh region");

  record(ADDR(printf), "libc", "printf()",
         "not in your binary at all: shared library, mapped in at startup");

  record(ADDR(&local), "stack", "local",
         "automatic variable, gone the moment main() returns");
  record(ADDR(local_arr), "stack", "local_arr[64]",
         "arrays are laid out in the frame just like scalars");
  record(ADDR(&ptr_to_local), "stack", "ptr_to_local",
         "the POINTER is on the stack, and so is what it points at");
  record(ADDR(&argc), "stack", "argc",
         "parameters are part of the frame too");
  record(ADDR(argv[0]), "stack", "argv[0] string",
         "argv/environ strings sit at the very top of the stack");

  deeper(1);

  /* ---- the same data, sorted low address to high ---------------------- */

  qsort(items, n_items, sizeof items[0], by_addr);

  puts("\nsorted low -> high (this is the actual memory map):\n");
  for (size_t i = 0; i < n_items; i++) {
    if (i > 0) {
      unsigned long long gap = (unsigned long long)(items[i].addr - items[i-1].addr);
      unsigned long long kib = gap / 1024ULL;
      if (kib >= 1024ULL * 1024ULL * 1024ULL)
        printf("         %10llu TiB unmapped gap\n", kib / (1024ULL*1024ULL*1024ULL));
      else if (kib >= 1024ULL * 1024ULL)
        printf("         %10llu GiB unmapped gap\n", kib / (1024ULL*1024ULL));
      else if (kib >= 1024ULL)
        printf("         %10llu MiB unmapped gap\n", kib / 1024ULL);
    }
    printf("  %#18" PRIxPTR "  %-8s %s\n",
           items[i].addr, items[i].region, items[i].name);
  }

  puts("\nRun it again: every address moves (ASLR), but the order does not.");

  free(small);
  free(big);
  free(copy);
  return 0;
}
