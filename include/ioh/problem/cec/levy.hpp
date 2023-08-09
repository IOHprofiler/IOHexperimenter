#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    class Levy final : public CECProblem<Levy>
    {
    protected:

        double evaluate(const std::vector<double> &x) override
        {
            int nx = x.size();

            std::vector<double> w(nx);
            for (size_t i = 0; i < nx; ++i)
            {
                w[i] = 1.0 + (x[i] - 0.0) / 4.0;
            }

            double term1 = pow(sin(IOH_PI * w[0]), 2);
            double term3 = pow(w[nx - 1] - 1, 2) * (1 + pow(sin(2 * IOH_PI * w[nx - 1]), 2));

            double sum = 0.0;
            for (size_t i = 0; i < nx - 1; ++i)
            {
                double wi = w[i];
                double newv = pow(wi - 1, 2) * (1 + 10 * pow(sin(IOH_PI * wi + 1), 2));
                sum += newv;
            }

            double result = term1 + sum + term3;

            std::cout << "result: " << result << std::endl;

            return result;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            auto&& transformed_variables = this->basic_transform(x, 5.12 / 100, 0);
            return transformed_variables;
        }

    public:

        Levy(const int instance, const int n_variables) :
            CECProblem(5, instance, n_variables, "CEC_Levy")
        {
        }
    };
}
