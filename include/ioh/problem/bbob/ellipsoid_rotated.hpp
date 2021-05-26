#pragma once

#include "ellipsoid.hpp"

namespace ioh::problem::bbob
{
    class EllipsoidRotated final : public EllipsoidBase<EllipsoidRotated>

    {
    protected:
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, objective_.x);
            affine(x, transformation_state_.transformation_matrix,
                                                    transformation_state_.transformation_base);
            oscillate(x);
            return x;
        }

    public:
        EllipsoidRotated(const int instance, const int n_variables) :
            EllipsoidBase(10, instance, n_variables, "EllipsoidRotated")
        {
            static const auto condition = 1.0e6;
            for (auto i = 1; i < meta_data_.n_variables; ++i)
                transformation_state_.conditions[i] = pow(condition, transformation_state_.exponents.at(i));
        }
    };
}
