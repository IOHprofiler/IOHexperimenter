#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    class CompositionFunction2 final : public CECProblem<CompositionFunction2>
    {
    protected:

        double evaluate(const std::vector<double>& x) override
        {
            double f = cf02(x, this->variables_shifts_, this->linear_transformations_, 1);
            return f;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            return x;
        }

    public:

        inline static const int meta_problem_id = 10;
        inline static const std::string meta_name = "CEC_CompositionFunction2";

        CompositionFunction2(const int instance, const int n_variables) :
            CECProblem(meta_problem_id, instance, n_variables, meta_name)
        {
            this->set_optimum();
        }
    };
}
