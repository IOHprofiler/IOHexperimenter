#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{
    class Rastrigin final: public BBOB<Rastrigin>
    {
    protected:
        std::vector<double> evaluate(std::vector<double> &x) override
        {
            return {static_evaluate(x)};
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::coco;
            transform_vars_shift_evaluate_function(x, meta_data_.objective.x);
            transform_vars_oscillate_evaluate_function(x);
            transform_vars_asymmetric_evaluate_function(x, 0.2);
            transform_vars_conditioning_evaluate(x, 10.0);
            return x;
        }

    public:
        Rastrigin(const int instance, const int n_variables) :
            BBOB(3, instance, n_variables, "Rastrigin")
        {
        }

        static double static_evaluate(std::vector<double>& x)
        {
            auto sum1 = 0.0, sum2 = 0.0;

            for (const auto xi : x)
            {
                sum1 += cos(2.0 * transformation::coco::coco_pi * xi);
                sum2 += xi * xi;
            }
            if (std::isinf(sum2))
                return  sum2;

            return  10.0 * (static_cast<double>(x.size()) - sum1) + sum2 ;
        }

    };
}
