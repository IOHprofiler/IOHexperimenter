#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    template<typename T>
    class RastriginBase: public BBOProblem<T>
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
                return { sum2 };

            return 10.0 * (static_cast<double>(x.size()) - sum1) + sum2;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, this->objective_.x);
            oscillate(x);
            asymmetric(x, 0.2);
            conditioning(x, 10.0);
            return x;
        }

    public:
        RastriginBase(const int problem_id, const int instance, const int n_variables,  const std::string& name ) :
            BBOProblem<T>(problem_id, instance, n_variables, name)
        {
        }
    };

    class Rastrigin final: public RastriginBase<Rastrigin>
    {
    public:
        Rastrigin(const int instance, const int n_variables) :
            RastriginBase(3, instance, n_variables, "Rastrigin")
        {
        }
    };


}
