// Same benchmark as bench_append.c, but appending the numbers 1..N
// instead of the same 7 every time. Growing list from list5_grow.c:
// start at 16, realloc to cap * 1.25.

// clock_gettime is POSIX, not plain ISO C, so -std=c11 hides it
// without this line.
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define INITIAL_CAP 16
#define N 10000000

int *numbers;
int len = 0;
int cap = 0;

void make_room(void) {
  if (len < cap) {
    return;
  }
  int newcap = cap + cap / 4;
  if (newcap <= cap) {
    newcap = cap + 1;
  }
  int *bigger = realloc(numbers, (size_t)newcap * sizeof(int));
  if (bigger == NULL) {
    fprintf(stderr, "realloc failed\n");
    exit(1);
  }
  numbers = bigger;
  cap = newcap;
}

void append(int value) {
  make_room();
  numbers[len] = value;
  len++;
}

int main() {
  numbers = malloc(INITIAL_CAP * sizeof(int));
  if (numbers == NULL) {
    fprintf(stderr, "malloc failed\n");
    exit(1);
  }
  cap = INITIAL_CAP;

  struct timespec t0, t1;
  clock_gettime(CLOCK_MONOTONIC, &t0);
  for (int i = 1; i <= N; i++) {
    append(i);
  }
  clock_gettime(CLOCK_MONOTONIC, &t1);

  double elapsed = (double)(t1.tv_sec - t0.tv_sec)
                 + (double)(t1.tv_nsec - t0.tv_nsec) / 1e9;

  printf("c:      %d appends in %7.1f ms  (%6.1f ns each)  last=%d\n",
         len, elapsed * 1000, elapsed * 1e9 / N, numbers[len - 1]);

  free(numbers);
  return 0;
}
