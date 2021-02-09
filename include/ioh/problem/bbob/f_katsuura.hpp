#pragma once
#include "bbob_base.hpp"

namespace ioh {
    namespace problem {
        namespace bbob {
            class Katsuura : public bbob_base {
                const double penalty_factor_ = 1.0;
                std::vector<double> raw_x_;

            public:
                Katsuura(int instance_id = IOH_DEFAULT_INSTANCE,
                         int dimension = IOH_DEFAULT_DIMENSION)
                    : bbob_base(23, "Katsuura", instance_id, dimension),
                      raw_x_(dimension) {
                    set_number_of_variables(dimension);
                }


                void prepare_problem() override {
                    using namespace transformation::coco;

                    bbob2009_compute_xopt(xopt_, rseed_, n_);
                    bbob2009_compute_rotation(rot1_, rseed_ + 1000000, n_);
                    bbob2009_compute_rotation(rot2_, rseed_, n_);
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
                                    sqrt(100.0), exponent) * rot2_[k][j];
                            }
                        }
                    }
                }

                double internal_evaluate(
                    const std::vector<double> &x) override {

                    /* Computation core */
                    auto result = 1.0;
                    for (size_t i = 0; i < n_; ++i) {
                        double tmp = 0;
                        for (size_t j = 1; j < 33; ++j) {
                            const auto tmp2 = pow(2., static_cast<double>(j));
                            tmp += fabs(tmp2 * x[i] - floor(tmp2 * x[i] + 0.5))
                                / tmp2;
                        }
                        tmp = 1.0 + (static_cast<double>(static_cast<long>(i)) +
                                     1) * tmp;
                        /*result *= tmp;*/
                        /* Wassim TODO: delete once consistency check passed*/
                        result *= pow(
                            tmp, 10. / pow(static_cast<double>(n_), 1.2));
                    }
                    result = 10. / static_cast<double>(n_) / static_cast<double>
                             (n_) * (
                                 -1. + result);
                    return result;
                }

                void objectives_transformation(const std::vector<double> &x,
                                               std::vector<double> &y,
                                               const int transformation_id,
                                               const int instance_id) override {
                    transformation::coco::transform_obj_shift_evaluate_function(
                        y, fopt_);
                    transformation::coco::transform_obj_penalize_evaluate(
                        raw_x_, lower_bound_, upper_bound_, penalty_factor_, y);
                }

                void variables_transformation(std::vector<double> &x,
                                              const int transformation_id,
                                              const int instance_id) override {
                    raw_x_ = x;
                    transformation::coco::transform_vars_shift_evaluate_function(
                        x, xopt_);
                    transformation::coco::transform_vars_affine_evaluate_function(
                        x, m_, b_);
                }

                static Katsuura *create(int instance_id = IOH_DEFAULT_INSTANCE,
                                        int dimension = IOH_DEFAULT_DIMENSION) {
                    return new Katsuura(instance_id, dimension);
                }
            };
        }
    }
}
