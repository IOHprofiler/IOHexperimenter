#pragma once

#include "ellipsoid.hpp"

namespace ioh::problem::bbob
{
    //! Rotated ellipsoid problem id 10
    class EllipsoidRotated final : public EllipsoidBase<EllipsoidRotated>
    {
    protected:
        //! Transoform variables method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, optimum_.x);
            affine(x, transformation_state_.transformation_matrix,
                                                    transformation_state_.transformation_base);
            oscillate(x);
            return x;
        }

    public:
        /**
         * @brief Construct a new Ellipsoid Rotated object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem
         */
        EllipsoidRotated(const int instance, const int n_variables) :
            EllipsoidBase(10, instance, n_variables, "EllipsoidRotated")
        {
            static const auto condition = 1.0e6;
            for (auto i = 1; i < meta_data_.n_variables; ++i)
                transformation_state_.conditions[i] = pow(condition, transformation_state_.exponents.at(i));
        }
    };
}
