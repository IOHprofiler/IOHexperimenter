#pragma once

#include "rosenbrock.hpp"


namespace ioh::problem::bbob
{
    //! Rotated Rosenbrock function 9
    template<typename P = BBOB>
    class RosenbrockRotated final :  public RosenbrockBase<P>, BBOProblem<RosenbrockRotated>
    {
    protected:
        //! Variables transformation method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            transformation::variables::affine(x,
                this->transformation_state_.second_transformation_matrix, 
                this->transformation_state_.transformation_base);
            return x;
        }

    public:
        /**
         * @brief Construct a new Rosenbrock Rotated object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem
         */
        RosenbrockRotated(const int instance, const int n_variables) :
            RosenbrockBase<P>(9, instance, n_variables, "RosenbrockRotated")
        {
            const auto factor = std::max(1.0, std::sqrt(n_variables) / 8.0);
            for (auto i = 0; i < n_variables; ++i)
            {
                auto sum = 0.0;
                for (auto j = 0; j < n_variables; ++j)
                {
                    this->transformation_state_.second_transformation_matrix[i][j] = factor 
                        * this->transformation_state_.second_rotation[i][j];
                    sum += this->transformation_state_.second_rotation[j][i];
                }
                this->transformation_state_.transformation_base[i] = 0.5;
                this->optimum_.x[i] = sum / (2. * factor);
            }
        }
    };

    template class RosenbrockRotated<BBOB>;
}
