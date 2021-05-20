#pragma once

#include "rastrigin.hpp"

namespace ioh::problem::bbob
{
    class RastriginRotated final : public RastriginBase<RastriginRotated>
    {
    protected:
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, objective_.x);
            affine(x, transformation_state_.transformation_matrix, transformation_state_.transformation_base);
            oscillate(x);
            asymmetric(x, 0.2);
            affine(x, transformation_state_.second_transformation_matrix, transformation_state_.transformation_base);
            return x;
        }

    public:
        RastriginRotated(const int instance, const int n_variables) :
            RastriginBase(15, instance, n_variables, "RastriginRotated")
        {
        }
    };
}
