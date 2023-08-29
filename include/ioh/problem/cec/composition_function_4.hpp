#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    class CompositionFunction4 final : public CECProblem<CompositionFunction4>
    {
    protected:

        double evaluate(const std::vector<double>& x) override
        {
            double f = cf07(x, this->variables_shifts_, this->linear_transformations_, 1);
            return f;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            return x;
        }

    public:

        inline static const int meta_problem_id = 12;
        inline static const std::string meta_name = "CEC_CompositionFunction4";

        CompositionFunction4(const int instance, const int n_variables) :
            CECProblem(meta_problem_id, instance, n_variables, meta_name)
        {
            this->set_optimum();
        }
    };
}
