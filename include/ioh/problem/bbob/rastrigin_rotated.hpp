#pragma once

#include "rastrigin.hpp"

namespace ioh::problem::bbob
{
    //! Rotated Rastrigin problem id 15
    template<typename P = BBOB>
    class RastriginRotated final : public RastriginBase<P>, BBOProblem<RastriginRotated>
    {
    protected:
        //! Variables transformation method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, this->optimum_.x);
            affine(x, this->transformation_state_.transformation_matrix, this->transformation_state_.transformation_base);
            oscillate(x);
            asymmetric(x, 0.2);
            affine(x, this->transformation_state_.second_transformation_matrix, this->transformation_state_.transformation_base);
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
            RastriginBase<P>(15, instance, n_variables, "RastriginRotated")
        {
        }
    };

    template class RastriginRotated<BBOB>;
}
