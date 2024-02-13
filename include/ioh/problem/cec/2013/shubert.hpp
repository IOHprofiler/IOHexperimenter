#pragma once

#include "cec_problem.hpp"


namespace ioh::problem
{
    namespace cec2013
    {
        class Shubert final : public CEC2013Problem<Shubert>
        {
        protected:
            double evaluate(const std::vector<double> &x) override
            {
                double result = 1;
                for (size_t i = 0; i < x.size(); i++)
                {
                    double soma = 0.0;
                    for (int j = 1; j < 6; j++)
                    {
                        soma = soma + (j * std::cos((j + 1) * x[i] + j));   
                    }
                    result *= soma;
                }
                return -result;
            }

        public:
            Shubert(const int problem_id, const int instance, const int n_variables) :
                CEC2013Problem(problem_id, instance, n_variables, "CEC2013Shubert", -10.0, 10.0, static_cast<int>(std::pow(n_variables * 3, 3)))
            {
            }
        };
    } // namespace cec2013


    template <>
    inline InstanceBasedProblem::Constructors<cec2013::Shubert, int, int>
    InstanceBasedProblem::load_instances<cec2013::Shubert>(const std::optional<fs::path> &definitions_file)
    {
        return {{[](int, int) { return cec2013::Shubert(1106, 1, 2); }, 1106, std::nullopt},
                {[](int, int) { return cec2013::Shubert(1108, 1, 3); }, 1108, std::nullopt}
        };
    }

} // namespace ioh::problem