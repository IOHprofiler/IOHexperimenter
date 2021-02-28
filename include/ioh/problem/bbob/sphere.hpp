#pragma once
#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    class Sphere final: public BBOBBase<Sphere>
    {
    protected:
        std::vector<double> evaluate(std::vector<double>& x) override
        {
            std::vector<double> result{ 0.0 };
            for (const auto xi : x)
                result[0] += xi * xi;
            return result;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            transformation::coco::transform_vars_shift_evaluate_function(x, meta_data_.objective.x);
            return x;
        }
    public:
        Sphere(const int instance, const int n_variables) :
            BBOBBase(1, instance, n_variables, "Sphere")
        {
        }
    };
}