#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    class CompositionFunction1 final : public CECProblem<CompositionFunction1>
    {
    protected:

        double evaluate(const std::vector<double>& x) override
        {
            double f = cf01(x, this->variables_shifts_, this->linear_transformations_, 1);
            return f;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            return x;
        }

    public:

        inline static const int meta_problem_id = 1009;
        inline static const std::string meta_name = "CEC_CompositionFunction1";

        CompositionFunction1(const int instance, const int n_variables) :
            CECProblem(meta_problem_id, instance, n_variables, meta_name)
        {
            this->set_optimum();
        }
    };
}
