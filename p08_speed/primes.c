// Count the first N primes by simple trial division.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int is_prime(int n) {

  if (n < 2) {
    return 0;
  }

  for (int d = 2; d * d <= n; d++) {
    if (n % d == 0) {
      return 0;
    }
  }

  return 1;
}

int main(int argc, char *argv[]) {

  int n = 100000;
  if (argc > 1) {
    n = atoi(argv[1]);
  }

  clock_t start = clock();

  int count = 0;
  int last = 0;
  int candidate = 1;

  while (count < n) {
    candidate++;
    if (is_prime(candidate)) {
      count++;
      last = candidate;
    }
  }

  double seconds = (double)(clock() - start) / CLOCKS_PER_SEC;

  printf("C      : prime #%d = %d in %.3f s\n", n, last, seconds);

  return 0;
}
