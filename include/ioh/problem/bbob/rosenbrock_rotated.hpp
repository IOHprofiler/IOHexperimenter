#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"
#include "rosenbrock.hpp"


namespace ioh::problem::bbob
{
    class RosenbrockRotated final : public BBOB<RosenbrockRotated>

    {
    protected:
        std::vector<double> evaluate(std::vector<double> &x) override
        {
            return { Rosenbrock::static_evaluate(x) };
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            transformation::coco::transform_vars_affine_evaluate_function(x,
                transformation_state_.m, transformation_state_.b);
            return x;
        }

    public:
        RosenbrockRotated(const int instance, const int n_variables) :
            BBOB(9, instance, n_variables, "RosenbrockRotated")
        {
            const auto factor = std::max(1.0, std::sqrt(meta_data_.n_variables) / 8.0);
            for (auto i = 0; i < meta_data_.n_variables; ++i)
            {
                for (auto j = 0; j < meta_data_.n_variables; ++j)
                    transformation_state_.m[i][j] = factor * transformation_state_.rot2.at(i).at(j);
                transformation_state_.b[i] = 0.5;
            }
                
        }
    };
}
