#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <time.h>
#include <gmp.h>

#define N 50000UL

static double now(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

static void factorial_with_recursion(mpz_t f, unsigned long n) {
    if (n > 1) {
        factorial_with_recursion(f, n - 1);
        mpz_mul_ui(f, f, n);
    } else {
        mpz_set_ui(f, 1);
    }
}

int main(void) {
    // like `x = factorial(N)` in Python: the previous result stays
    // alive while the next one is computed, then gets replaced
    mpz_t x;
    mpz_init(x);

    for (int run = 0; run < 6; run++) {
        mpz_t f;
        mpz_init(f);

        double start_time = now();
        factorial_with_recursion(f, N);
        double end_time = now();

        mpz_swap(x, f);
        mpz_clear(f);
        printf("run %d: %.3fs\n", run + 1, end_time - start_time);
    }

    mpz_clear(x);
    return 0;
}
