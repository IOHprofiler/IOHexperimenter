#pragma once
#include "bbob_base.hpp"

namespace ioh {
    namespace problem {
        namespace bbob {
            class AttractiveSector : public bbob_base {
            public:
                AttractiveSector(int instance_id = IOH_DEFAULT_INSTANCE,
                                 int dimension = IOH_DEFAULT_DIMENSION)
                    : bbob_base(6, "Attractive_Sector", instance_id,
                                dimension) {
                    //TODO: This has to be called inside the child class need we to think of something here
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

                void objectives_transformation(const std::vector<double> &x,
                                               std::vector<double> &y,
                                               const int transformation_id,
                                               const int instance_id) override {
                    transformation::coco::transform_obj_oscillate_evaluate(y);
                    transformation::coco::transform_obj_power_evaluate(y, 0.9);
                    transformation::coco::transform_obj_shift_evaluate_function(
                        y, fopt_);
                }

                void variables_transformation(std::vector<double> &x,
                                              const int transformation_id,
                                              const int instance_id) override {
                    transformation::coco::transform_vars_shift_evaluate_function(
                        x, xopt_);
                    transformation::coco::transform_vars_affine_evaluate_function(
                        x, m_, b_);
                }

                double internal_evaluate(
                    const std::vector<double> &x) override {
                    auto result = 0.0;
                    for (size_t i = 0; i < n_; ++i) {
                        if (xopt_[i] * x[i] > 0.0)
                            result += 100.0 * 100.0 * x[i] * x[i];
                        else
                            result += x[i] * x[i];
                    }
                    return result;
                }

                static AttractiveSector *create(
                    int instance_id = IOH_DEFAULT_INSTANCE,
                    int dimension = IOH_DEFAULT_DIMENSION) {
                    return new AttractiveSector(instance_id, dimension);
                }
            };
        }
    }
}