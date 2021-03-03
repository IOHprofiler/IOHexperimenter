#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    class Discus final : public BBOProblem<Discus>
    {
    protected:
        std::vector<double> evaluate(std::vector<double> &x) override
        {
            static const auto condition = 1.0e6;
            std::vector<double> result = {condition * x.at(0) * x.at(0)};
            for (auto i = 1; i < meta_data_.n_variables; ++i)
                result[0] += x.at(i) * x.at(i);
            return result;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::coco;
            transform_vars_shift_evaluate_function(x, objective_.x);
            transform_vars_affine_evaluate_function(x, transformation_state_.transformation_matrix,
                                                    transformation_state_.transformation_base);
            transform_vars_oscillate_evaluate_function(x);
            return x;
        }

    public:
        Discus(const int instance, const int n_variables) :
            BBOProblem(11, instance, n_variables, "Discus")
        {
        }
    };
}
