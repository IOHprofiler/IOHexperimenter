#pragma once
#include "bbob_base.hpp"

namespace ioh {
    namespace problem {
        namespace bbob {
            class Ellipsoid : public bbob_base {
            public:
                Ellipsoid(int instance_id = IOH_DEFAULT_INSTANCE,
                          int dimension = IOH_DEFAULT_DIMENSION)
                    : bbob_base(2, "Ellipsoid", instance_id, dimension) {
                    set_number_of_variables(dimension);
                }

                void prepare_problem() override {
                    transformation::coco::bbob2009_compute_xopt(
                        xopt_, rseed_, n_);
                }

                double internal_evaluate(
                    const std::vector<double> &x) override {
                    static const auto condition = 1.0e6;
                    auto result = x[0] * x[0];
                    for (auto i = 1; i < n_; ++i) {
                        const auto exponent =
                            1.0 * static_cast<double>(static_cast<long>(i)) / (
                                static_cast<double>(
                                    static_cast<long>(n_)) - 1.0);
                        result += pow(condition, exponent) * x[i] * x[i];
                    }
                    return result;
                }


                void variables_transformation(std::vector<double> &x,
                                              const int transformation_id,
                                              const int instance_id) override {
                    transformation::coco::transform_vars_shift_evaluate_function(
                        x, xopt_);
                    transformation::coco::transform_vars_oscillate_evaluate_function(
                        x);
                }

                static Ellipsoid *create(int instance_id = IOH_DEFAULT_INSTANCE,
                                         int dimension =
                                             IOH_DEFAULT_DIMENSION) {
                    return new Ellipsoid(instance_id, dimension);
                }
            };
        }
    }
}
