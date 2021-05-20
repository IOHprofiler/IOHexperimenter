#pragma once
#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    class Sphere final: public BBOProblem<Sphere>
    {
    protected:
        double evaluate(const std::vector<double>& x) override
        {
            auto result = 0.0;
            for (const auto xi : x)
                result += xi * xi;
            return result;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            transformation::variables::subtract(x, objective_.x);
            return x;
        }
    public:
        Sphere(const int instance, const int n_variables) :
            BBOProblem(1, instance, n_variables, "Sphere")
        {
        }
    };
}