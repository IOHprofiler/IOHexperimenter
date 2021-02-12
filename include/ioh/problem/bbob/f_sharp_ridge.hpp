#pragma once
#include "bbob_base.hpp"


namespace ioh {
    namespace problem {
        namespace bbob {
            class Sharp_Ridge : public bbob_base {
            public:
                Sharp_Ridge(int instance_id = IOH_DEFAULT_INSTANCE,
                            int dimension = IOH_DEFAULT_DIMENSION)
                    : bbob_base(13, "Sharp_Ridge", instance_id, dimension) {
                    set_number_of_variables(dimension);
                }

                void prepare_problem() override {
                    transformation::coco::bbob2009_compute_xopt(
                        xopt_, rseed_, n_);
                    transformation::coco::bbob2009_compute_rotation(
                        rot1_, rseed_ + 1000000, n_);
                    transformation::coco::bbob2009_compute_rotation(
                        rot2_, rseed_, n_);
                    for (auto i = 0; i < n_; ++i) {
                        b_[i] = 0.0;
                        for (auto j = 0; j < n_; ++j) {
                            m_[i][j] = 0.0;
                            for (auto k = 0; k < n_; ++k) {
                                const auto exponent =
                                    1.0 * static_cast<int>(k) / (
                                        static_cast<double>(static_cast<long>(n_
                                        ))
                                        - 1.0);
                                m_[i][j] += rot1_[i][k] * pow(
                                    sqrt(10.0), exponent) * rot2_[k][j];
                            }
                        }
                    }
                }

                double internal_evaluate(
                    const std::vector<double> &x) override {
                    static const auto alpha = 100.0;
                    const double vars_40 = 1;
                    /* generalized: n_variables <= 40 ? 1 : n_variables / 40.0; */
                    auto i = 0;

                    auto result = 0.0;
                    for (i = static_cast<int>(ceil(vars_40)); i < n_; ++i) {
                        result += x[i] * x[i];
                    }
                    result = alpha * sqrt(result / vars_40);
                    for (i = 0; i < static_cast<int>(ceil(vars_40)); ++i) {
                        result += x[i] * x[i] / vars_40;
                    }

                    return result;
                }

                void variables_transformation(std::vector<double> &x,
                                              const int transformation_id,
                                              const int instance_id) override {
                    transformation::coco::transform_vars_shift_evaluate_function(
                        x, xopt_);
                    transformation::coco::transform_vars_affine_evaluate_function(
                        x, m_, b_);
                }

                static Sharp_Ridge *create(
                    int instance_id = IOH_DEFAULT_INSTANCE,
                    int dimension = IOH_DEFAULT_DIMENSION) {
                    return new Sharp_Ridge(instance_id, dimension);
                }
            };
        }
    }
}
