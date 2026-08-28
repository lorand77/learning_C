// Count the first N primes with a sieve of Eratosthenes.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// The nth prime is smaller than n * (ln n + ln ln n) for n >= 6.
long limit_for(int n) {

  if (n < 6) {
    return 15;
  }

  double x = n;
  return (long)(x * (log(x) + log(log(x)))) + 1;
}

int main(int argc, char *argv[]) {

  int n = 100000;
  if (argc > 1) {
    n = atoi(argv[1]);
  }

  clock_t start = clock();

  long limit = limit_for(n);

  char *is_prime = malloc(limit + 1);
  memset(is_prime, 1, limit + 1);
  is_prime[0] = 0;
  is_prime[1] = 0;

  for (long i = 2; i * i <= limit; i++) {
    if (is_prime[i]) {
      for (long j = i * i; j <= limit; j += i) {
        is_prime[j] = 0;
      }
    }
  }

  int count = 0;
  long last = 0;
  for (long i = 2; i <= limit; i++) {
    if (is_prime[i]) {
      count++;
      if (count == n) {
        last = i;
        break;
      }
    }
  }

  double seconds = (double)(clock() - start) / CLOCKS_PER_SEC;

  printf("C      : prime #%d = %ld in %.3f s\n", n, last, seconds);

  free(is_prime);

  return 0;
}
