#pragma once
#include "bbob_base.hpp"


namespace ioh {
    namespace problem {
        namespace bbob {
            class Rosenbrock_Rotated : public bbob_base {
                double factor_;
            public:
                Rosenbrock_Rotated(int instance_id = IOH_DEFAULT_INSTANCE,
                                   int dimension = IOH_DEFAULT_DIMENSION)
                    : bbob_base(9, "Rosenbrock_Rotated", instance_id,
                                dimension),
                      factor_(std::max(1.0, std::sqrt(dimension) / 8.0)) {
                    set_number_of_variables(dimension);
                }

                void prepare_problem() override {
                    transformation::coco::bbob2009_compute_rotation(
                        rot1_, rseed_, n_);
                    for (auto row = 0; row < n_; ++row) {
                        for (auto column = 0; column < n_; ++column) {
                            m_[row][column] = factor_ * rot1_[row][column];
                        }
                        b_[row] = 0.5;
                    }
                }

                double internal_evaluate(
                    const std::vector<double> &x) override {
                    auto s1 = 0.0, s2 = 0.0;
                    for (size_t i = 0; i < n_ - 1; ++i) {
                        auto tmp = x[i] * x[i] - x[i + 1];
                        s1 += tmp * tmp;
                        tmp = x[i] - 1.0;
                        s2 += tmp * tmp;
                    }
                    return 100.0 * s1 + s2;
                }


                void variables_transformation(std::vector<double> &x,
                                              const int transformation_id,
                                              const int instance_id) override {
                    transformation::coco::transform_vars_affine_evaluate_function(
                        x, m_, b_);
                }

                static Rosenbrock_Rotated *create(
                    int instance_id = IOH_DEFAULT_INSTANCE,
                    int dimension = IOH_DEFAULT_DIMENSION) {
                    return new Rosenbrock_Rotated(instance_id, dimension);
                }
            };
        }
    }
}
