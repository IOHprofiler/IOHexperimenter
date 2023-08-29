#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    class CompositionFunction3 final : public CECProblem<CompositionFunction3>
    {
    protected:

        double evaluate(const std::vector<double>& x) override
        {
            double f = cf06(x, this->variables_shifts_, this->linear_transformations_, 1);
            return f;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            return x;
        }

    public:

        inline static const int meta_problem_id = 11;
        inline static const std::string meta_name = "CEC_CompositionFunction3";

        CompositionFunction3(const int instance, const int n_variables) :
            CECProblem(meta_problem_id, instance, n_variables, meta_name)
        {
            this->set_optimum();
        }
    };
}
