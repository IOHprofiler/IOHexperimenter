#pragma once
#include "bbob_base.hpp"

namespace ioh {
    namespace problem {
        namespace bbob {
            class Griewank_RosenBrock : public bbob_base {
                double scales = 0.0;

            public:
                Griewank_RosenBrock(int instance_id = IOH_DEFAULT_INSTANCE,
                                    int dimension = IOH_DEFAULT_DIMENSION)
                    : bbob_base(19, "Griewank_RosenBrock", instance_id,
                                dimension) {
                    set_number_of_variables(dimension);
                }

                void prepare_problem() override {
                    for (auto i = 0; i < n_; ++i) {
                        xopt_[i] = -0.5;
                    }
                    transformation::coco::bbob2009_compute_rotation(
                        rot1_, rseed_, n_);
                    scales = 1. > sqrt(static_cast<double>(n_)) / 8.
                                 ? 1.
                                 : sqrt(static_cast<double>(n_)) / 8.;
                    for (auto i = 0; i < n_; ++i) {
                        for (auto j = 0; j < n_; ++j) {
                            rot1_[i][j] *= scales;
                        }
                    }
                    transformation::coco::bbob2009_copy_rotation_matrix(
                        rot1_, m_, b_, n_);
                }

                double internal_evaluate(
                    const std::vector<double> &x) override {
                    auto result = 0.0;
                    for (auto i = 0; i < n_ - 1; ++i) {
                        const auto c1 = x[i] * x[i] - x[i + 1];
                        const auto c2 = 1.0 - x[i];
                        const auto tmp = 100.0 * c1 * c1 + c2 * c2;
                        result += tmp / 4000. - cos(tmp);
                    }
                    return 10. + 10. * result / static_cast<double>(n_ - 1);
                }

                void variables_transformation(std::vector<double> &x,
                                              const int transformation_id,
                                              const int instance_id) override {
                    transformation::coco::transform_vars_affine_evaluate_function(
                        x, m_, b_);
                    transformation::coco::transform_vars_shift_evaluate_function(
                        x, xopt_);
                }


                static Griewank_RosenBrock *create(
                    int instance_id = IOH_DEFAULT_INSTANCE,
                    int dimension = IOH_DEFAULT_DIMENSION) {
                    return new Griewank_RosenBrock(instance_id, dimension);
                }
            };
        }
    }
}
