#pragma once

#include <cassert>
#include <vector>
#include <cmath>

namespace ioh {
    namespace problem {


        namespace transformation {
            namespace coco {
                static const double coco_pi = 3.14159265358979323846;
                /** @brief Maximal dimension used in BBOB2009. */
#define SUITE_BBOB2009_MAX_DIM 40

                /** @brief Computes the minimum of the two values. */
                static double bbob2009_fmin(double a, double b) {
                    return a < b ? a : b;
                }

                /** @brief Computes the maximum of the two values. */
                static double bbob2009_fmax(double a, double b) {
                    return a > b ? a : b;
                }

                /** @brief Rounds the given value. */
                static double bbob2009_round(double x) {
                    return floor(x + 0.5);
                }

                /**
                 * @brief Generates N uniform random numbers using inseed as the seed and stores them in r.
                 */
                static void bbob2009_unif(std::vector<double> &r, size_t N,                                          long inseed) {
                    long tmp;
                    long rgrand[32];
                    long i;
                    r = std::vector<double>(N);

                    if (inseed < 0)
                        inseed = -inseed;
                    if (inseed < 1)
                        inseed = 1;
                    auto aktseed = inseed;
                    for (i = 39; i >= 0; i--) {
                        tmp = static_cast<int>(floor(
                            static_cast<double>(aktseed) / static_cast<double>(
                                127773)));
                        aktseed = 16807 * (aktseed - tmp * 127773) - 2836 * tmp;
                        if (aktseed < 0)
                            aktseed = aktseed + 2147483647;
                        if (i < 32)
                            rgrand[i] = aktseed;
                    }
                    auto aktrand = rgrand[0];
                    for (i = 0; i < static_cast<long>(N); i++) {
                        tmp = static_cast<int>(floor(
                            static_cast<double>(aktseed) / static_cast<double>(
                                127773)));
                        aktseed = 16807 * (aktseed - tmp * 127773) - 2836 * tmp;
                        if (aktseed < 0)
                            aktseed = aktseed + 2147483647;
                        tmp = static_cast<int>(floor(
                            static_cast<double>(aktrand) / static_cast<double>(
                                67108865)));
                        aktrand = rgrand[tmp];
                        rgrand[tmp] = aktseed;
                        r[i] = static_cast<double>(aktrand) / 2.147483647e9;
                        if (r[i] == 0.) {
                            r[i] = 1e-99;
                        }
                    }
                }

                /**
                 * @brief Converts from packed matrix storage to an array of array of double representation.
                 */
                static void bbob2009_reshape(
                    std::vector<std::vector<double>> &B,
                    const std::vector<double> &vector,
                    const size_t m, const size_t n) {
                    B = std::vector<std::vector<double>>(m);
                    for (size_t i = 0; i < m; i++) {
                        B[i] = std::vector<double>(n);
                        for (size_t j = 0; j < n; j++) {
                            B[i][j] = vector[j * m + i];
                        }
                    }
                }

                /**
                 * @brief Generates N Gaussian random numbers using the given seed and stores them in g.
                 */
                static void bbob2009_gauss(std::vector<double> &g,
                                           const size_t N,
                                           const long seed) {
                    g = std::vector<double>(N);
                    std::vector<double> uniftmp(6000);
                    assert(2 * N < 6000);

                    bbob2009_unif(uniftmp, 2 * N, seed);

                    for (size_t i = 0; i < N; i++) {
                        g[i] = sqrt(-2 * std::log(uniftmp[i])) * cos(
                                   2 * coco_pi * uniftmp[N + i]);
                        if (g[i] == 0.)
                            g[i] = 1e-99;
                    }
                }

                /**
                 * @brief Computes a DIM by DIM rotation matrix based on seed and stores it in B.
                 */
                static void bbob2009_compute_rotation(
                    std::vector<std::vector<double>> &B,
                    const long seed, const long DIM) {
                    /* To ensure temporary data fits into gvec */
                    double prod;
                    std::vector<double> gvect(2000);
                    long k; /* Loop over pairs of column vectors. */

                    bbob2009_gauss(gvect, (DIM * DIM), seed);
                    bbob2009_reshape(B, gvect, DIM, DIM);
                    /*1st coordinate is row, 2nd is column.*/

                    for (long i = 0; i < DIM; i++) {
                        for (long j = 0; j < i; j++) {
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

                static void bbob2009_copy_rotation_matrix(
                    const std::vector<std::vector<double>> &rot,
                    std::vector<std::vector<double>> &M, std::vector<double> &b,
                    const size_t DIM) {
                    //double *current_row;

                    for (size_t row = 0; row < DIM; ++row) {
                        for (size_t column = 0; column < DIM; ++column) {
                            M[row][column] = rot[row][column];
                        }
                        b[row] = 0.0;
                    }
                }

                /**
                 * @brief Randomly computes the location of the global optimum.
                 */
                static void bbob2009_compute_xopt(
                    std::vector<double> &xopt, const long seed,
                    const long DIM) {
                    bbob2009_unif(xopt, DIM, seed);
                    for (long i = 0; i < DIM; i++) {
                        xopt[i] = 8 * floor(1e4 * xopt[i]) / 1e4 - 4;
                        if (xopt[i] == 0.0)
                            xopt[i] = -1e-5;
                    }
                }

                /**
                 * @brief Randomly chooses the objective offset for the given function and get.
                 */
                static double bbob2009_compute_fopt(const size_t function,
                                                    const size_t instance) {
                    long rseed;
                    std::vector<double> gval, gval2;

                    if (function == 4)
                        rseed = 3;
                    else if (function == 18)
                        rseed = 17;
                    else if (function == 101 || function == 102 || function ==
                             103 || function ==
                             107
                             || function == 108 || function == 109)
                        rseed = 1;
                    else if (function == 104 || function == 105 || function ==
                             106 || function ==
                             110
                             || function == 111 || function == 112)
                        rseed = 8;
                    else if (function == 113 || function == 114 || function ==
                             115)
                        rseed = 7;
                    else if (function == 116 || function == 117 || function ==
                             118)
                        rseed = 10;
                    else if (function == 119 || function == 120 || function ==
                             121)
                        rseed = 14;
                    else if (function == 122 || function == 123 || function ==
                             124)
                        rseed = 17;
                    else if (function == 125 || function == 126 || function ==
                             127)
                        rseed = 19;
                    else if (function == 128 || function == 129 || function ==
                             130)
                        rseed = 21;
                    else
                        rseed = static_cast<long>(function);

                    const auto rrseed =
                        rseed + static_cast<long>(10000 * instance);
                    bbob2009_gauss(gval, 1, rrseed);
                    bbob2009_gauss(gval2, 1, rrseed + 1);
                    return bbob2009_fmin(
                        1000., bbob2009_fmax(
                            -1000., bbob2009_round(
                                        100. * 100. * gval[0] / gval2[0]) /
                                    100.));
                }
            }
        }
    }
}
