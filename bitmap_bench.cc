/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>

#include "bitmap.h"
#include "shared.h"

double densityL = 0.1;
double densityR = 0.1;

uint64 numOnesL = 0;
uint64 numOnesR = 0;

const int numIters = 10;
const int numQueries = 1000000;
uint64 queries[numQueries];

uint32 seed = 1;

inline uint32 xRand()
{
    return seed = (279470273ULL * seed) % 4294967291ULL;
}

inline uint64 xRand64()
{
    return (uint64) xRand() << 32 | xRand();
}

uint64* createRandomBits(uint64 nbits, uint32 thresholdL, uint32 thresholdR)
{
    fprintf(stderr, "nbits to create: %" PRIu64 "\n", nbits);
    fprintf(stderr, "allocated bits: %" PRIu64 " bytes\n", nbits/8);

    uint64* bits = NULL;
    assert(posix_memalign((void **) &bits, 4096, nbits / 8) == 0);

    for (uint64 i = 0; i < nbits / 2; i++) {
        if (xRand() < thresholdL) { 
            bits[i / 64] |= 1UL << (i % 64);
            ++numOnesL;
        } else {
            bits[i / 64] &= ~(1ULL << (i % 64));
        }
    }
    for (uint64 i = nbits / 2; i < nbits; i++) {
        if (xRand() < thresholdR) {
            bits[i / 64] |= 1ULL << (i % 64);
            ++numOnesR;
        } else {
            bits[i / 64] &= ~(1ULL << (i % 64));
        }
    }

    return bits;
}

enum benchmode {
    BENCH_RANK,
    BENCH_SELECT,
};

int main(int argc, char **argv)
{
    extern int optind;
    int ch;

    uint64 nbits;
    benchmode mode = BENCH_RANK;

    while ((ch = getopt(argc, argv, "sn:d:")) != -1) {
        switch (ch) {
        case 's':
            mode = BENCH_SELECT;
            break;
        case 'n':
            nbits = atoi(optarg);
            nbits = 1ULL << nbits;
            break;
        case 'd':
            densityL = densityR = atof(optarg);
            break;
        }
    }

    printf("benchmode: %s\n", mode == BENCH_RANK ? "rank" : "select");

    uint32 thresholdL = (uint32) (UINT32_MAX * densityL);
    uint32 thresholdR = (uint32) (UINT32_MAX * densityR);

    uint64* bits = createRandomBits(nbits, thresholdL, thresholdR);
    BitmapPoppy* bitmap = new BitmapPoppy(bits, nbits);
    uint64 dummy = 0x1234567890ABCDEF;

    if (mode == BENCH_RANK) {
        for (int i = 0; i < numQueries; i++) {
            queries[i] = xRand64() % nbits + 1;
        }
    } else {
        assert(mode == BENCH_SELECT);

        for (int i = 0; i < numQueries / 2; i++) {
            queries[i] = xRand64() % numOnesL + 1;
        }
        for (int i = numQueries / 2; i < numQueries; i++) {
            queries[i] = xRand64() % numOnesR + 1 + numOnesL;
        }
    }

    struct timeval tv_start, tv_end;
    gettimeofday(&tv_start, NULL);

    if (mode == BENCH_RANK) {
        for (int iter = 0; iter < numIters; iter++)
            for (int i = 0; i < numQueries; i++)
                dummy ^= bitmap->rank(queries[i]);
    } else {
        assert(mode == BENCH_SELECT);

        for (int iter = 0; iter < numIters; iter++)
            for (int i = 0; i < numQueries; i++)
                dummy ^= bitmap->select(queries[i]);
    }
    gettimeofday(&tv_end, NULL);

    double elapsed_seconds = timeval_diff(&tv_start, &tv_end);
    printf("%" PRIu64 " ops, %.2f seconds, ns/op: %.2f\n", 
           (uint64) numIters * numQueries, 
           elapsed_seconds,
           elapsed_seconds * 1000000000 / ((uint64) numIters * numQueries));

    if (dummy == 42) printf("42\n");

    return 0;
}
