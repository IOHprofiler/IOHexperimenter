#pragma once

#include <vector>

#include "ioh/common.hpp"


namespace ioh {
    namespace problem {
        namespace transformation {
            namespace methods {
                /// \fn xor_compute(const int x1, const int x2)
                ///
                /// Exclusive or operation on x1, x2. x1 and x2 should either 0 or 1.
                static int xor_compute(int x1, int x2) {
                    return static_cast<int>(x1 != x2);
                }

                /// \fn transform_vars_xor(std::vector<int> &x, const int seed)
                ///
                /// Applying xor operations on x with a uniformly random bit string.
                inline void transform_vars_xor(std::vector<int> &x,
                                               const int seed) {
                    std::vector<double> random_x;
                    const int n = x.size();
                    common::Random::uniform(n, seed, random_x);

                    for (auto i = 0; i < n; ++i) {
                        const auto xor_value = static_cast<int>(
                            2 * floor(1e4 * random_x[i]) / 1e4);
                        x[i] = xor_compute(x[i], xor_value);
                    }
                }

                static int sigma_compute(const std::vector<int> &x,
                                         const int pos) {
                    return x[pos];
                }

                /// \fn transform_vars_sigma(std::vector<int> &x, const int seed)
                ///
                /// Disrupting the order of x.
                inline void transform_vars_sigma(
                    std::vector<int> &x, const int seed) {
                    std::vector<int> index;
                    std::vector<double> random_x;
                    const int N = x.size();

                    const auto copy_x = x;

                    index.reserve(N);
                    for (auto i = 0; i != N; ++i) {
                        index.push_back(i);
                    }

                    common::Random::uniform(N, seed, random_x);
                    for (auto i = 0; i != N; ++i) {
                        const auto t = static_cast<int>(floor(random_x[i] * N));
                        const auto temp = index[0];
                        index[0] = index[t];
                        index[t] = temp;
                    }
                    for (auto i = 0; i < N; ++i) {
                        x[i] = sigma_compute(copy_x, index[i]);
                    }
                }

                /// \fn void transform_obj_scale(std::vector<double> &y, const int seed)
                /// \brief transformation 'a * f(x)'.
                inline void transform_obj_scale(std::vector<double> &y,
                                                const int seed) {
                    std::vector<double> scale;
                    common::Random::uniform(1, seed, scale);
                    scale[0] = scale[0] * 1e4 / 1e4 * 4.8 + 0.2;
                    for (size_t i = 0; i < y.size(); ++i) {
                        y[i] = y[i] * scale[0];
                    }
                }

                /// \fn void transform_obj_shift(std::vector<double> &y, const int seed)
                /// \brief transformation 'f(x) + b'.
                inline void transform_obj_shift(std::vector<double> &y,
                                                const int seed) {
                    std::vector<double> shift;
                    common::Random::uniform(1, seed, shift);
                    shift[0] = shift[0] * 1e4 / 1e4 * 2000 - 1000;
                    for (size_t i = 0; i < y.size(); ++i) {
                        y[i] = y[i] + shift[0];
                    }
                }

                /// \fn void transform_obj_scale(std::vector<double> &y, const int seed)
                /// \brief transformation 'a * f(x)'.
                inline void transform_obj_scale(double &y, const int seed) {
                    std::vector<double> scale;
                    common::Random::uniform(1, seed, scale);
                    scale[0] = scale[0] * 1e4 / 1e4 * 4.8 + 0.2;
                    y = y * scale[0];
                }

                /// \fn void transform_obj_shift(std::vector<double> &y, const int seed)
                /// \brief transformation 'f(x) + b'.
                inline void transform_obj_shift(double &y, const int seed) {
                    std::vector<double> shift;
                    common::Random::uniform(1, seed, shift);
                    shift[0] = shift[0] * 1e4 / 1e4 * 2000 - 1000;
                    y = y + shift[0];
                }
            };
        }
    }
}
