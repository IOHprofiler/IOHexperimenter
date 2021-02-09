#pragma once

#include "legacy.hpp"
#include <vector>


namespace ioh {
    namespace problem {


        namespace transformation {
            namespace coco {
                static void transform_vars_affine_evaluate_function(
                    std::vector<double> &x,
                    const std::vector<
                        std::vector<double>> &M,
                    const std::vector<double> &
                    b) {
                    const auto number_of_variables = x.size();
                    //double *cons_values;
                    //int is_feasible;
                    auto temp_x = x;
                    for (size_t i = 0; i < number_of_variables; ++i) {
                        /* data->M has problem->number_of_variables columns and inner_problem->number_of_variables rows. */
                        x[i] = b[i];
                        for (size_t j = 0; j < number_of_variables; ++j) {
                            x[i] += temp_x[j] * M[i][j];
                        }
                    }
                }

                static void transform_vars_asymmetric_evaluate_function(
                    std::vector<double> &x, const double beta) {
                    const auto number_of_variables = x.size();
                    for (size_t i = 0; i < number_of_variables; ++i) {
                        if (x[i] > 0.0) {
                            const auto exponent = 1.0
                                                  + beta * static_cast<double>(
                                                      static_cast<long>(i)) / (
                                                      static_cast<double>(
                                                          static_cast<
                                                              long>(
                                                              number_of_variables
                                                          )) - 1.0) *
                                                  sqrt(x[i]);
                            x[i] = pow(x[i], exponent);
                        } else {
                            x[i] = x[i];
                        }
                    }
                }

                static void
                transform_vars_brs_evaluate(std::vector<double> &x) {
                    const auto number_of_variables = x.size();

                    for (size_t i = 0; i < number_of_variables; ++i) {
                        /* Function documentation says we should compute 10^(0.5 *
                         * (i-1)/(D-1)). Instead we compute the equivalent
                         * sqrt(10)^((i-1)/(D-1)) just like the legacy code.
                         */
                        auto factor = pow(
                            sqrt(10.0),
                            static_cast<double>(static_cast<long>(i)) / (
                                static_cast<double>(static_cast<long>(
                                    number_of_variables)) - 1.0));
                        /* Documentation specifies odd indices and starts indexing
                         * from 1, we use all even indices since C starts indexing
                         * with 0.
                         */
                        if (x[i] > 0.0 && i % 2 == 0) {
                            factor *= 10.0;
                        }
                        x[i] = factor * x[i];
                    }
                }

                static void transform_vars_conditioning_evaluate(
                    std::vector<double> &x, const double alpha) {
                    const auto number_of_variables = x.size();

                    for (size_t i = 0; i < number_of_variables; ++i) {
                        /* OME: We could precalculate the scaling coefficients if we
                         * really wanted to.
                         */
                        x[i] = pow(
                                   alpha,
                                   0.5 * static_cast<double>(static_cast<long>(i
                                   )) / (
                                       static_cast<double>(static_cast<long>(
                                           number_of_variables)) - 1.0))
                               * x[i];
                    }
                }

                static void transform_vars_oscillate_evaluate_function(
                    std::vector<double> &x) {
                    static const auto alpha = 0.1;
                    const auto number_of_variables = x.size();
                    double tmp, base;

                    for (size_t i = 0; i < number_of_variables; ++i) {
                        if (x[i] > 0.0) {
                            tmp = log(x[i]) / alpha;
                            base = exp(
                                tmp + 0.49 * (sin(tmp) + sin(0.79 * tmp)));
                            x[i] = pow(base, alpha);
                        } else if (x[i] < 0.0) {
                            tmp = log(-x[i]) / alpha;
                            base = exp(
                                tmp + 0.49 * (sin(0.55 * tmp) + sin(0.31 * tmp)
                                ));
                            x[i] = -pow(base, alpha);
                        } else {
                            x[i] = 0.0;
                        }
                    }
                }

                static void transform_vars_scale_evaluate(
                    std::vector<double> &x,
                    const double factor) {
                    const auto number_of_variables = x.size();

                    for (size_t i = 0; i < number_of_variables; ++i) {
                        x[i] = factor * x[i];
                    }
                }

                static void transform_vars_shift_evaluate_function(
                    std::vector<double> &x,
                    const std::vector<double> &
                    offset) {
                    const auto number_of_variables = x.size();

                    for (size_t i = 0; i < number_of_variables; ++i) {
                        x[i] = x[i] - offset[i];
                    }
                }

                static void transform_vars_x_hat_evaluate(
                    std::vector<double> &x,
                    const long seed) {
                    const auto number_of_variables = x.size();
                    std::vector<double> tmp_x;

                    bbob2009_unif(tmp_x, static_cast<long>(number_of_variables),
                                  seed);

                    for (size_t i = 0; i < number_of_variables; ++i) {
                        if (tmp_x[i] < 0.5) {
                            x[i] = -x[i];
                        } else {
                            x[i] = x[i];
                        }
                    }
                }

                static void transform_vars_x_hat_generic_evaluate(
                    std::vector<double> &x,
                    const std::vector<double> &
                    sign_vector) {
                    const auto number_of_variables = x.size();

                    for (size_t i = 0; i < number_of_variables; ++i) {
                        x[i] = 2.0 * sign_vector[i] * x[i];
                    }
                }

                static void transform_vars_z_hat_evaluate(
                    std::vector<double> &x,
                    const std::vector<double> &xopt) {
                    const auto number_of_variables = x.size();
                    auto temp_x = x;
                    x[0] = temp_x[0];

                    for (size_t i = 1; i < number_of_variables; ++i) {
                        x[i] = temp_x[i] + 0.25 * (
                                   temp_x[i - 1] - 2.0 * fabs(xopt[i - 1]));
                    }
                }
            }
        }
    }
}
