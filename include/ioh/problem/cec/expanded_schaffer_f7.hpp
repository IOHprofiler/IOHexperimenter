#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    class ExpandedSchafferF7 final : public CECProblem<ExpandedSchafferF7>
    {
    protected:

        double evaluate(const std::vector<double>& y) override
        {
            double&& f = schaffer_F7_func(y);
            return f;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            std::vector<double> y(x.size()), z(x.size());

            scale_and_rotate(x, z, y, this->variables_shifts_[0], this->linear_transformations_[0], 0.5 / 100.0, 1, 1);

            return y;
        }

    public:

        inline static const int meta_problem_id = 3;
        inline static const std::string meta_name = "CEC_ExpandedSchafferF7";

        ExpandedSchafferF7(const int instance, const int n_variables) :
            CECProblem(meta_problem_id, instance, n_variables, meta_name)
        {
            this->set_optimum();
        }
    };
}
