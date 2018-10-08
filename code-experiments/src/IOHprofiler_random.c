/**
 * @file IOHprofiler_random.c
 * @brief Definitions of functions regarding IOHprofiler random numbers.
 *
 * @note This file contains non-C89-standard types (such as uint32_t and uint64_t), which should
 * eventually be fixed.
 */

#include <math.h>

#include "IOHprofiler.h"

#define IOHprofiler_NORMAL_POLAR /* Use polar transformation method */

#define IOHprofiler_SHORT_LAG 273
#define IOHprofiler_LONG_LAG 607

/**
 * @brief A structure containing the state of the IOHprofiler random generator.
 */
struct IOHprofiler_random_state_s {
    double x[IOHprofiler_LONG_LAG];
    size_t index;
};

/**
 * @brief A lagged Fibonacci random number generator.
 *
 * This generator is nice because it is reasonably small and directly generates double values. The chosen
 * lags (607 and 273) lead to a generator with a period in excess of 2^607-1.
 */
static void IOHprofiler_random_generate(IOHprofiler_random_state_t *state) {
    size_t i;
    for (i = 0; i < IOHprofiler_SHORT_LAG; ++i) {
        double t = state->x[i] + state->x[i + (IOHprofiler_LONG_LAG - IOHprofiler_SHORT_LAG)];
        if (t >= 1.0)
            t -= 1.0;
        state->x[i] = t;
    }
    for (i = IOHprofiler_SHORT_LAG; i < IOHprofiler_LONG_LAG; ++i) {
        double t = state->x[i] + state->x[i - IOHprofiler_SHORT_LAG];
        if (t >= 1.0)
            t -= 1.0;
        state->x[i] = t;
    }
    state->index = 0;
}

IOHprofiler_random_state_t *IOHprofiler_random_new(uint32_t seed) {
    IOHprofiler_random_state_t *state = (IOHprofiler_random_state_t *)IOHprofiler_allocate_memory(sizeof(*state));
    size_t i;
    /* Expand seed to fill initial state array. */
    for (i = 0; i < IOHprofiler_LONG_LAG; ++i) {
        /* Uses uint64_t to silence the compiler ("shift count negative or too big, undefined behavior" warning) */
        state->x[i] = ((double)seed) / (double)(((uint64_t)1UL << 32) - 1);
        /* Advance seed based on simple RNG from TAOCP */
        seed = (uint32_t)1812433253UL * (seed ^ (seed >> 30)) + ((uint32_t)i + 1);
    }
    state->index = 0;
    return state;
}

void IOHprofiler_random_free(IOHprofiler_random_state_t *state) {
    IOHprofiler_free_memory(state);
}

double IOHprofiler_random_uniform(IOHprofiler_random_state_t *state) {
    /* If we have consumed all random numbers in our archive, it is time to run the actual generator for one
   * iteration to refill the state with 'LONG_LAG' new values. */
    if (state->index >= IOHprofiler_LONG_LAG)
        IOHprofiler_random_generate(state);
    return state->x[state->index++];
}

/**
 * Instead of using the (expensive) polar method, we may cheat and abuse the central limit theorem. The sum
 * of 12 uniform random values has mean 6, variance 1 and is approximately normal. Subtract 6 and you get
 * an approximately N(0, 1) random number.
 */
double IOHprofiler_random_normal(IOHprofiler_random_state_t *state) {
    double normal;
#ifdef IOHprofiler_NORMAL_POLAR
    const double u1 = IOHprofiler_random_uniform(state);
    const double u2 = IOHprofiler_random_uniform(state);
    normal = sqrt(-2 * log(u1)) * cos(2 * IOHprofiler_pi * u2);
#else
    int i;
    normal = 0.0;
    for (i = 0; i < 12; ++i) {
        normal += IOHprofiler_random_uniform(state);
    }
    normal -= 6.0;
#endif
    return normal;
}

/* Be hygienic (for amalgamation) and undef lags. */
#undef IOHprofiler_SHORT_LAG
#undef IOHprofiler_LONG_LAG
