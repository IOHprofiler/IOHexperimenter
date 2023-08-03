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
            if (std::isinf(sum2))
                return sum2 ;

            return 10.0 * (static_cast<double>(x.size()) - sum1) + sum2;
        }

    public:
        Rastrigin(const int instance, const int n_variables) :
            CECProblem(100 /*Should be unique*/, instance, n_variables, "CECRastrigin", "2022-SO-BO/CEC2022/C-Code/input_data/M_4_D2.txt")
        {
            // if(n_variables != 2 && n_variables != 10 && n_variables != 20) {
            //     std::ostringstream message;
            //     message << "Error: n_variables should be 2, 10, or 20. Provided n_variables: " << n_variables;
            //     throw std::invalid_argument(message.str());
            // }

            optimum_.x = std::vector<double>(n_variables, 1);
            optimum_.y = evaluate(optimum_.x);
            // optimum_.x = reset_transform_variables(optimum_.x);
            // optimum_.y = transform_objectives(optimum_.y);
        }
    };
}
