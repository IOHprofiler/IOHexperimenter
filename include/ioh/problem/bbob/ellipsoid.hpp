#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{
    class Ellipsoid final: public BBOB, AutomaticFactoryRegistration<Ellipsoid, RealProblem>

    {
    protected:
        std::vector<double> evaluate(std::vector<double> &x) override
        {
            static const auto condition = 1.0e6;
            std::vector<double> result = {x[0] * x[0]};
            for (auto i = 1; i < meta_data_.n_variables; ++i)
            {
                const auto exponent =
                    1.0 * static_cast<double>(i) / (static_cast<double>(meta_data_.n_variables) - 1.0);
                result[0] += pow(condition, exponent) * x[i] * x[i];
            }
            return result;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::coco;
            transform_vars_shift_evaluate_function(x, meta_data_.objective.x);
            transform_vars_oscillate_evaluate_function(x);
            return x;
        }

    public:
        Ellipsoid(const int instance, const int n_variables) :
            BBOB(2, instance, n_variables, "Ellipsoid")
        {
        }
    };
}
