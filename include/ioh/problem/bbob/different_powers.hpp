#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    class DifferentPowers final : public BBOProblem<DifferentPowers>

    {
    protected:
        std::vector<double> evaluate(const std::vector<double> &x) override
        {
            auto sum = 0.0;
            for (auto i = 0; i < meta_data_.n_variables; ++i)
                sum += pow(fabs(x.at(i)), transformation_state_.exponents.at(i));
            return {sqrt(sum)};
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::coco;
            transform_vars_shift_evaluate_function(x, objective_.x);
            transform_vars_affine_evaluate_function(x, transformation_state_.transformation_matrix, transformation_state_.transformation_base);
            return x;
        }

    public:
        DifferentPowers(const int instance, const int n_variables) :
            BBOProblem(14, instance, n_variables, "DifferentPowers")
        {
            for (auto i = 0; i < meta_data_.n_variables; ++i)
                transformation_state_.exponents[i] = 2.0 + 4.0 * transformation_state_.exponents.at(i);
        }
    };
}
