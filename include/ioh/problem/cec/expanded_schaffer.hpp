#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    class ExpandedSchaffer final : public CECProblem<ExpandedSchaffer>
    {
    protected:

        double evaluate(const std::vector<double> &x) override
        {
            double result = 0.0;
            double temp1, temp2;
            int nx = x.size();

            for (int i = 0; i < nx - 1; ++i)
            {
                temp1 = sin(sqrt(x[i] * x[i] + x[i + 1] * x[i + 1]));
                temp1 = temp1 * temp1;
                temp2 = 1.0 + 0.001 * (x[i] * x[i] + x[i + 1] * x[i + 1]);
                result += 0.5 + (temp1 - 0.5) / (temp2 * temp2);
            }

            temp1 = sin(sqrt(x[nx - 1] * x[nx - 1] + x[0] * x[0]));
            temp1 = temp1 * temp1;
            temp2 = 1.0 + 0.001 * (x[nx - 1] * x[nx - 1] + x[0] * x[0]);
            result += 0.5 + (temp1 - 0.5) / (temp2 * temp2);

            return result;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            auto&& transformed_variables = this->basic_transform(x, 0.5 / 100, 0);
            return transformed_variables;
        }

    public:

        ExpandedSchaffer(const int instance, const int n_variables) :
            CECProblem(3, instance, n_variables, "CEC_ExpandedSchaffer")
        {
        }
    };
}
