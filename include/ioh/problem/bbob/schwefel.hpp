#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    class Schwefel final : public BBOProblem<Schwefel>

    {
        std::vector<double> negative_offset_;
        std::vector<double> positive_offset_;
    protected:
        std::vector<double> evaluate(std::vector<double> &x) override
        {
            static const auto correction = 418.9828872724339;
            std::vector<double> result = {0.0};
            auto penalty = 0.0;
            for (const auto &xi : x)
            {
                const auto out_of_bounds = fabs(xi) - 500.0;
                if (out_of_bounds > 0.0)
                    penalty += out_of_bounds * out_of_bounds;

                result[0] += xi * sin(sqrt(fabs(xi)));
            }
            result[0] = 0.01 * (penalty + correction - result.at(0) / static_cast<double>(meta_data_.n_variables));
            return result;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::coco;
            transform_vars_x_hat_evaluate(x, transformation_state_.seed);
            transform_vars_scale_evaluate(x, 2);
            transform_vars_z_hat_evaluate(x, objective_.x);
            transform_vars_shift_evaluate_function(x, positive_offset_);
            transform_vars_conditioning_evaluate(x, 10.0);
            transform_vars_shift_evaluate_function(x, negative_offset_);
            transform_vars_scale_evaluate(x, 100);
            return x;
        }

    public:
        Schwefel(const int instance, const int n_variables) :
            BBOProblem(20, instance, n_variables, "Schwefel"),
            negative_offset_(n_variables),
            positive_offset_(n_variables)
        {
            transformation::coco::bbob2009_unif(negative_offset_, n_variables, transformation_state_.seed);

            for (auto i = 0; i < n_variables; ++i)
                objective_.x[i] = (negative_offset_.at(i) < 0.5 ? -1 : 1) * 0.5 * 4.2096874637;

            for (auto i = 0; i < n_variables; ++i)
            {
                negative_offset_[i] = -2 * fabs(objective_.x.at(i));
                positive_offset_[i] = 2 * fabs(objective_.x.at(i));
            }
        }
    };
}
