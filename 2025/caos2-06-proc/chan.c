#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <x86intrin.h>

static volatile unsigned char sink;

static const uint8_t secret = (uint8_t)'B';

static inline uint64_t rdtsc_serialized(void) {
    unsigned aux;
    _mm_lfence();
    uint64_t t = __rdtscp(&aux);
    _mm_lfence();
    return t;
}

static inline uint64_t time_load(volatile unsigned char *p) {
    uint64_t t0 = rdtsc_serialized();
    sink ^= *p;
    uint64_t t1 = rdtsc_serialized();
    return t1 - t0;
}

static int cmp_u64(const void *a, const void *b) {
    uint64_t x = *(const uint64_t*)a;
    uint64_t y = *(const uint64_t*)b;
    return (x > y) - (x < y);
}

static uint64_t median_u64(uint64_t *arr, int n) {
    qsort(arr, (size_t)n, sizeof(arr[0]), cmp_u64);
    return arr[n / 2];
}

int main(void) {
    enum { N = 256, STRIDE = 4096, CAL_ITERS = 401 };

    static unsigned char probe[N * STRIDE];
    uint64_t times[N];

    for (int i = 0; i < N; i++) {
        probe[i * STRIDE] = (unsigned char)i;
    }

    volatile unsigned char *cal = &probe[123 * STRIDE];

    // прогрев
    (void)*cal;

    uint64_t cached_samples[CAL_ITERS];
    uint64_t flushed_samples[CAL_ITERS];

    for (int k = 0; k < CAL_ITERS; k++) {
        // Cached
        (void)*cal;
        cached_samples[k] = time_load(cal);

        // Flushed
        _mm_clflush((void*)cal);
        _mm_mfence();
        flushed_samples[k] = time_load(cal);
    }

    uint64_t cached_med  = median_u64(cached_samples, CAL_ITERS);
    uint64_t flushed_med = median_u64(flushed_samples, CAL_ITERS);

    // порог между cached/flushed
    uint64_t threshold = (cached_med + flushed_med) / 2;

    printf("Calibration (median of %d samples):\n", CAL_ITERS);
    printf("  cached_med  = %llu cycles\n", (unsigned long long)cached_med);
    printf("  flushed_med = %llu cycles\n", (unsigned long long)flushed_med);
    printf("  threshold   = %llu cycles\n\n", (unsigned long long)threshold);

    for (int i = 0; i < N; i++) {
        _mm_clflush(&probe[i * STRIDE]);
    }
    _mm_mfence();

    volatile unsigned char tmp = probe[(size_t)secret * STRIDE];
    sink ^= tmp;

    for (int k = 0; k < N; k++) {
        int i = (k * 167 + 13) & 255;
        volatile unsigned char *p = &probe[(size_t)i * STRIDE];
        times[i] = time_load(p);
    }

    printf("Touched index: %u ('%c')\n\n",
           (unsigned)secret, (secret >= 32 && secret < 127) ? (char)secret : '.');

    int best_i = -1;
    uint64_t best_dt = (uint64_t)-1;
    for (int i = 0; i < N; i++) {
        if (times[i] < best_dt) {
            best_dt = times[i];
            best_i = i;
        }
    }

    printf("Guessed index: %d ('%c') cycles=%llu\n\n",
           best_i, (best_i >= 32 && best_i < 127) ? (char)best_i : '.',
           (unsigned long long)best_dt);

    printf("All timings (hex index):\n");
    for (int i = 0; i < N; i++) {
        printf("%02X:%4llu", i, (unsigned long long)times[i]);
        if ((i + 1) % 8 == 0) printf("\n");
        else printf("\t");
    }
    if (N % 8 != 0) printf("\n");

    printf("\nCache hits (< %llu cycles):\n", (unsigned long long)threshold);

    int hits = 0;
    for (int i = 0; i < N; i++) {
        if (times[i] < threshold) {
            printf("  i=%3d (0x%02X, '%c')  dt=%4llu\n",
                   i, i, (i >= 32 && i < 127) ? (char)i : '.',
                   (unsigned long long)times[i]);
            hits++;
        }
    }

    printf("\nTotal hits: %d\n", hits);

    if (sink == 0xFF) puts("sink");

    return 0;
}
