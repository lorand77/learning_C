#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <time.h>

// 12! = 479001600 is the largest factorial that fits in a 32-bit int
// (INT_MAX is about 2.1e9, 13! is about 6.2e9)
#define N 12
#define EXPECTED 479001600
#define REPS 1000000

static double now(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

static int factorial_with_recursion(int n) {
    if (n > 1) {
        return n * factorial_with_recursion(n - 1);
    } else {
        return 1;
    }
}

int main(void) {
    // volatile so the compiler can't precompute 12! or skip the calls
    volatile int n = N;
    volatile int x = 0;

    for (int run = 0; run < 6; run++) {
        double start_time = now();
        for (int rep = 0; rep < REPS; rep++) {
            x = factorial_with_recursion(n);
        }
        double end_time = now();

        if (x != EXPECTED) {
            printf("wrong result: %d\n", x);
            return 1;
        }
        double total = end_time - start_time;
        printf("run %d: %.3fs (%.1f ns per call)\n", run + 1, total, total / REPS * 1e9);
    }

    return 0;
}
