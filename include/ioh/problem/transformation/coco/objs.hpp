#pragma once

#include "legacy.hpp"
#include <vector>


namespace ioh {
    namespace problem {


        namespace transformation {
            namespace coco {
                static void transform_obj_norm_by_dim_evaluate(
                    const int dimension,
                    std::vector<double> &y) {
                    y[0] *= bbob2009_fmin(
                        1, 40. / static_cast<double>(dimension));
                    /* Wassim: might want to use a function (with no 40) here that we can put in a helpers file */
                }

                static void transform_obj_oscillate_evaluate(
                    std::vector<double> &y) {
                    static const auto factor = 0.1;
                    const auto number_of_objectives = y.size();

                    for (size_t i = 0; i < number_of_objectives; i++) {
                        if (y[i] != 0) {
                            const auto log_y = log(fabs(y[i])) / factor;
                            if (y[i] > 0) {
                                y[i] = pow(
                                    exp(log_y + 0.49 * (sin(log_y) + sin(
                                            0.79 * log_y))),
                                    factor);
                            } else {
                                y[i] = -pow(
                                    exp(log_y + 0.49 * (sin(0.55 * log_y) + sin(
                                            0.31 * log_y))),
                                    factor);
                            }
                        }
                    }
                }

                /**
                 * @brief Evaluates the transformation.
                 */
                static void transform_obj_penalize_evaluate(
                    const std::vector<double> &x,
                    const double lower_bounds,
                    const double upper_bounds,
                    const double factor,
                    std::vector<double> &y) {
                    auto penalty = 0.0;
                    size_t i;
                    const auto number_of_objectives = y.size();
                    const auto n = x.size();

                    for (i = 0; i < n; ++i) {
                        const auto c1 = x[i] - upper_bounds;
                        const auto c2 = lower_bounds - x[i];
                        if (c1 > 0.0) {
                            penalty += c1 * c1;
                        } else if (c2 > 0.0) {
                            penalty += c2 * c2;
                        }
                    }

                    for (i = 0; i < number_of_objectives; ++i) {
                        y[i] += factor * penalty;
                    }
                }


                static void transform_obj_power_evaluate(std::vector<double> &y,
                    const double exponent) {
                    const auto number_of_objectives = y.size();

                    for (size_t i = 0; i < number_of_objectives; i++) {
                        y[i] = pow(y[i], exponent);
                    }
                }

                static void transform_obj_scale_evaluate_function(
                    std::vector<double> &y, const double factor) {
                    const auto number_of_objectives = y.size();

                    for (size_t i = 0; i < number_of_objectives; i++)
                        y[i] *= factor;
                }

                static void transform_obj_shift_evaluate_function(
                    std::vector<double> &y, const double offset) {
                    const auto number_of_objectives = y.size();

                    for (size_t i = 0; i < number_of_objectives; i++)
                        y[i] += offset;
                }
            }
        }
    }
}
