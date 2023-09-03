#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    class Levy final : public CECProblem<Levy>
    {
    protected:

        double evaluate(const std::vector<double>& z) override
        {
            double&& f = levy_func(z);
            return f;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            std::vector<double> y(x.size()), z(x.size());

            scale_and_rotate(x, z, y, this->variables_shifts_[0], this->linear_transformations_[0], 5.12 / 100.0, 1, 1);

            return z;
        }

    public:

        inline static const int meta_problem_id = 1005;
        inline static const std::string meta_name = "CEC_Levy";

        Levy(const int instance, const int n_variables) :
            CECProblem(meta_problem_id, instance, n_variables, meta_name)
        {
            this->set_optimum();
        }
    };
}
