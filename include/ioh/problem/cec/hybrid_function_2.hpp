#pragma once

#include "cec_problem.hpp"
#include "functions.hpp"

namespace ioh::problem::cec
{
    class HybridFunction2 final : public CECProblem<HybridFunction2>
    {
    protected:

        double evaluate(const std::vector<double>& x) override
        {
            double f = hf10(x, this->variables_shift_, this->linear_transformation_, this->input_permutation_, 1, 1);
            return f;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            return x;
        }

    public:

        inline static const int meta_problem_id = 7;
        inline static const std::string meta_name = "CEC_HybridFunction2";

        HybridFunction2(const int instance, const int n_variables) :
            CECProblem(meta_problem_id, instance, n_variables, meta_name)
        {
            this->set_optimum();
        }
    };
}
