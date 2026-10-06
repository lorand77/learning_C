#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <time.h>

// 20! = 2432902008176640000 is the largest factorial that fits in a
// 64-bit long (LONG_MAX is about 9.2e18, 21! is about 5.1e19)
#define N 20
#define EXPECTED 2432902008176640000L
#define REPS 1000000

static double now(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

static long factorial(long n) {
    long f = 1;
    for (long i = 2; i <= n; i++) {
        f = f * i;
    }
    return f;
}

int main(void) {
    // volatile so the compiler can't precompute 20! or skip the calls
    volatile long n = N;
    volatile long x = 0;

    for (int run = 0; run < 6; run++) {
        double start_time = now();
        for (int rep = 0; rep < REPS; rep++) {
            x = factorial(n);
        }
        double end_time = now();

        if (x != EXPECTED) {
            printf("wrong result: %ld\n", x);
            return 1;
        }
        double total = end_time - start_time;
        printf("run %d: %.3fs (%.1f ns per call)\n", run + 1, total, total / REPS * 1e9);
    }

    return 0;
}
