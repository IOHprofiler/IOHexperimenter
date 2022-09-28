#pragma once

#include "rastrigin.hpp"

namespace ioh::problem::bbob
{
    //! Rotated Rastrigin problem id 15
    class RastriginRotated final : public RastriginBase<RastriginRotated>
    {
    protected:
        //! Variables transformation method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, optimum_.x);
            affine(x, transformation_state_.transformation_matrix, transformation_state_.transformation_base);
            oscillate(x);
            asymmetric(x, 0.2);
            affine(x, transformation_state_.second_transformation_matrix, transformation_state_.transformation_base);
            return x;
        }

    public:
        /**
         * @brief Construct a new Rastrigin Rotated object
         * 
        * @param instance instance id
         * @param n_variables the dimension of the problem
         */
        RastriginRotated(const int instance, const int n_variables) :
            RastriginBase(15, instance, n_variables, "RastriginRotated")
        {
        }
    };
}
