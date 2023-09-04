#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    class Rastrigin final : public CECProblem<Rastrigin>
    {
    protected:

        double evaluate(const std::vector<double> &x) override
        {
            auto sum1 = 0.0, sum2 = 0.0;
            for (const auto xi : x)
            {
                sum1 += cos(2.0 * IOH_PI * xi);
                sum2 += xi * xi;
            }
            if (std::isinf(sum2)) { return sum2; }

            auto result = 10.0 * (static_cast<double>(x.size()) - sum1) + sum2;

            return result;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            auto&& transformed_variables = this->basic_transform(x, 5.12 / 100, 0);
            return transformed_variables;
        }

    public:

        Rastrigin(const int instance, const int n_variables) :
            CECProblem(4, instance, n_variables, "CEC_Rastrigin")
        {
        }
    };
}
