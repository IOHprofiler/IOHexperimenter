#pragma once
#include "bbob_base.hpp"


namespace ioh {
    namespace problem {
        namespace bbob {
            class Schwefel : public bbob_base {
                const double condition_ = 10.0;
                std::vector<double> negative_offset_;
                std::vector<double> positive_offset_;

            public:
                Schwefel(int instance_id = IOH_DEFAULT_INSTANCE,
                         int dimension = IOH_DEFAULT_DIMENSION)
                    : bbob_base(20, "Schwefel", instance_id, dimension),
                      negative_offset_(dimension),
                      positive_offset_(dimension) {
                    set_number_of_variables(dimension);
                }

                void prepare_problem() override {
                    transformation::coco::bbob2009_unif(
                        negative_offset_, n_, rseed_);
                    for (auto i = 0; i < n_; ++i)
                        xopt_[i] = (negative_offset_[i] < 0.5 ? -1 : 1) * 0.5 *
                                   4.2096874637;

                    for (auto i = 0; i < n_; ++i) {
                        negative_offset_[i] = -2 * fabs(xopt_[i]);
                        positive_offset_[i] = 2 * fabs(xopt_[i]);
                    }
                }

                double internal_evaluate(
                    const std::vector<double> &x) override {
                    int i = 0;

                    /* Boundary handling*/
                    auto penalty = 0.0;
                    for (i = 0; i < n_; ++i) {
                        const auto tmp = fabs(x[i]) - 500.0;
                        if (tmp > 0.0) {
                            penalty += tmp * tmp;
                        }
                    }

                    /* Computation core */
                    auto sum = 0.0;
                    for (i = 0; i < n_; ++i) {
                        sum += x[i] * sin(sqrt(fabs(x[i])));
                    }

                    return 0.01 * (
                               penalty + 418.9828872724339 - sum / static_cast<
                                   double>(n_));
                }


                void variables_transformation(std::vector<double> &x,
                                              const int transformation_id,
                                              const int instance_id) override {
                    transformation::coco::transform_vars_x_hat_evaluate(
                        x, rseed_);
                    transformation::coco::transform_vars_scale_evaluate(x, 2);
                    transformation::coco::transform_vars_z_hat_evaluate(
                        x, xopt_);
                    transformation::coco::transform_vars_shift_evaluate_function(
                        x, positive_offset_);
                    transformation::coco::transform_vars_conditioning_evaluate(
                        x, 10.0);
                    transformation::coco::transform_vars_shift_evaluate_function(
                        x, negative_offset_);
                    transformation::coco::transform_vars_scale_evaluate(x, 100);
                }

                static Schwefel *create(int instance_id = IOH_DEFAULT_INSTANCE,
                                        int dimension = IOH_DEFAULT_DIMENSION) {
                    return new Schwefel(instance_id, dimension);
                }
            };
        }
    }
}
