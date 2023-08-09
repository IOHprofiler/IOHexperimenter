#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    class Rosenbrock final : public CECProblem<Rosenbrock>
    {
    protected:

        double evaluate(const std::vector<double> &x) override
        {
            double result = 0.0;

            std::vector<double> shifted = x; // To replace z with x and still work with the original values
            shifted[0] += 1.0; // Shift to origin

            for (size_t i = 0; i < x.size() - 1; i++)
            {
                shifted[i + 1] += 1.0; // Shift to origin
                double tmp1 = shifted[i] * shifted[i] - shifted[i + 1];
                double tmp2 = shifted[i] - 1.0;
                result += 100.0 * tmp1 * tmp1 + tmp2 * tmp2;
            }

            std::cout << "result: " << result << std::endl;

            return result;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            auto&& transformed_variables = this->basic_transform(x, 2.048 / 100, 1);
            return transformed_variables;
        }

    public:

        Rosenbrock(const int instance, const int n_variables) :
            CECProblem(2, instance, n_variables, "CEC_Rosenbrock")
        {
        }
    };
}
