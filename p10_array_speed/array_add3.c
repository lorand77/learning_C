// Make an array of N numbers 1, 2, ..., N.
// Then add 7 to every element, and time only that loop.

// Adds -fsanitize=address,undefined checks (3x runtime)

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 10000000

int main() {

  // An array of 10000000 ints is 40 MB, which is too big for the stack,
  // so we ask the operating system for the memory with malloc().
  int *arr = malloc(N * sizeof(int));

  // Fill the array with 1, 2, ..., N. This part is not timed.
  for (int i = 0; i < N; i++) {
    arr[i] = i + 1;
  }

  // This is the loop we care about: add 7 to every element.
  clock_t start = clock();

  for (int i = 0; i < N; i++) {
    arr[i] = arr[i] + 7;
  }

  double seconds = (double)(clock() - start) / CLOCKS_PER_SEC;

  printf("C      : add 7 to %d elements in %.4f s\n", N, seconds);

  free(arr);
  return 0;
}
