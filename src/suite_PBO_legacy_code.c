/**
 * @file suite_IOHprofiler_legacy_code.c
 * @brief Legacy code from required to replicate the IOHprofiler functions.
 *
 * All of this code should only be used by the suite_IOHprofiler functions to provide compatibility to the
 * legacy code. New test beds should strive to use the new IOHprofiler facilities for random number generation etc.
 */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "IOHprofiler.h"

/** @brief Maximal dimension used in IOHprofiler. */
#define SUITE_IOHprofiler_MAX_DIM 5000

/** @brief Computes the minimum of the two values. */
static double IOHprofiler_fmin(double a, double b) {
    return (a < b) ? a : b;
}

/** @brief Computes the maximum of the two values. */
static double IOHprofiler_fmax(double a, double b) {
    return (a > b) ? a : b;
}

/** @brief Rounds the given value. */
static double IOHprofiler_round(double x) {
    return floor(x + 0.5);
}

/**
 * @brief Allocates a n by m matrix structured as an array of pointers to double arrays.
 */
static double **IOHprofiler_allocate_matrix(const size_t n, const size_t m) {
    double **matrix = NULL;
    size_t i;
    matrix = (double **)IOHprofiler_allocate_memory(sizeof(double *) * n);
    for (i = 0; i < n; ++i) {
        matrix[i] = IOHprofiler_allocate_vector(m);
    }
    return matrix;
}

/**
 * @brief Frees the matrix structured as an array of pointers to double arrays.
 */
static void IOHprofiler_free_matrix(double **matrix, const size_t n) {
    size_t i;
    for (i = 0; i < n; ++i) {
        if (matrix[i] != NULL) {
            IOHprofiler_free_memory(matrix[i]);
            matrix[i] = NULL;
        }
    }
    IOHprofiler_free_memory(matrix);
}

/**
 * @brief Generates N uniform random numbers using inseed as the seed and stores them in r.
 */
static void IOHprofiler_unif(double *r, size_t N, long inseed) {
    /* generates N uniform numbers with starting seed */
    long aktseed;
    long tmp;
    long rgrand[32];
    long aktrand;
    long i;

    if (inseed < 0)
        inseed = -inseed;
    if (inseed < 1)
        inseed = 1;
    aktseed = inseed;
    for (i = 39; i >= 0; i--) {
        tmp = (int)floor((double)aktseed / (double)127773);
        aktseed = 16807 * (aktseed - tmp * 127773) - 2836 * tmp;
        if (aktseed < 0)
            aktseed = aktseed + 2147483647;
        if (i < 32)
            rgrand[i] = aktseed;
    }
    aktrand = rgrand[0];
    for (i = 0; i < N; i++) {
        tmp = (int)floor((double)aktseed / (double)127773);
        aktseed = 16807 * (aktseed - tmp * 127773) - 2836 * tmp;
        if (aktseed < 0)
            aktseed = aktseed + 2147483647;
        tmp = (int)floor((double)aktrand / (double)67108865);
        aktrand = rgrand[tmp];
        rgrand[tmp] = aktseed;
        r[i] = (double)aktrand / 2.147483647e9;
        if (r[i] == 0.) {
            r[i] = 1e-99;
        }
    }
    return;
}

/**
 * @brief Converts from packed matrix storage to an array of array of double representation.
 */
static double **IOHprofiler_reshape(double **B, double *vector, const size_t m, const size_t n) {
    size_t i, j;
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            B[i][j] = vector[j * m + i];
        }
    }
    return B;
}

/**
 * @brief Generates N Gaussian random numbers using the given seed and stores them in g.
 */
static void IOHprofiler_gauss(double *g, const size_t N, const long seed) {
    size_t i;
    double uniftmp[6000];
    assert(2 * N < 6000);
    IOHprofiler_unif(uniftmp, 2 * N, seed);

    for (i = 0; i < N; i++) {
        g[i] = sqrt(-2 * log(uniftmp[i])) * cos(2 * IOHprofiler_pi * uniftmp[N + i]);
        if (g[i] == 0.)
            g[i] = 1e-99;
    }
    return;
}

/**
 * @brief Computes a DIM by DIM rotation matrix based on seed and stores it in B.
 */
static void IOHprofiler_compute_rotation(double **B, const long seed, const size_t DIM) {
    /* To ensure temporary data fits into gvec */
    double prod;
    double gvect[2000];
    long i, j, k; /* Loop over pairs of column vectors. */

    assert(DIM * DIM < 2000);

    IOHprofiler_gauss(gvect, DIM * DIM, seed);
    IOHprofiler_reshape(B, gvect, DIM, DIM);
    /*1st coordinate is row, 2nd is column.*/

    for (i = 0; i < DIM; i++) {
        for (j = 0; j < i; j++) {
            prod = 0;
            for (k = 0; k < DIM; k++)
                prod += B[k][i] * B[k][j];
            for (k = 0; k < DIM; k++)
                B[k][i] -= prod * B[k][j];
        }
        prod = 0;
        for (k = 0; k < DIM; k++)
            prod += B[k][i] * B[k][i];
        for (k = 0; k < DIM; k++)
            B[k][i] /= sqrt(prod);
    }
}

static void IOHprofiler_copy_rotation_matrix(double **rot, double *M, double *b, const size_t DIM) {
    size_t row, column;
    double *current_row;

    for (row = 0; row < DIM; ++row) {
        current_row = M + row * DIM;
        for (column = 0; column < DIM; ++column) {
            current_row[column] = rot[row][column];
        }
        b[row] = 0.0;
    }
}

/**
 * @brief Randomly computes the location of the global optimum.
 */
static void IOHprofiler_compute_xopt(int *xopt, const long seed, const size_t DIM) {
    long i;
    double *xopttemp;
    xopttemp = IOHprofiler_allocate_vector(DIM);
    IOHprofiler_unif(xopttemp, DIM, seed);
    for (i = 0; i < DIM; i++) {
        xopt[i] = (int)(2 * floor(1e4 * xopttemp[i]) / 1e4 / 1);
    }
    IOHprofiler_free_memory(xopttemp);
}

/**
 * @brief Randomly computes the location of the global optimum.
 */
static void IOHprofiler_compute_xopt_double(double *xopt, const long seed, const size_t DIM) {
    long i;
    IOHprofiler_unif(xopt, DIM, seed);
    for (i = 0; i < DIM; i++) {
        xopt[i] = floor(1e4 * xopt[i]) / 1e4;
        if (xopt[i] == 0.0)
            xopt[i] = -1e-5;
    }
}

/**
 * @brief Randomly chooses the objective offset for the given function and instance.
 */
static double IOHprofiler_compute_fopt(const size_t function, const size_t instance) {
  long rseed, rrseed;
  double gval, gval2;


  rseed = (long) function;

  rrseed = rseed + (long) (10000 * instance);
  IOHprofiler_gauss(&gval, 1, rrseed);
  IOHprofiler_gauss(&gval2, 1, rrseed + 1);
  return IOHprofiler_fmin(1000., IOHprofiler_fmax(-1000., IOHprofiler_round(100. * 100. * gval / gval2) / 100.));
}
