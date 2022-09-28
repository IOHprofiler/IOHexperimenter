#pragma once

#include "rosenbrock.hpp"


namespace ioh::problem::bbob
{
    //! Rotated Rosenbrock function 9
    class RosenbrockRotated final : public RosenbrockBase<RosenbrockRotated>
    {
    protected:
        //! Variables transformation method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            transformation::variables::affine(x,
                transformation_state_.second_transformation_matrix, 
                transformation_state_.transformation_base);
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
            RosenbrockBase(9, instance, n_variables, "RosenbrockRotated")
        {
            const auto factor = std::max(1.0, std::sqrt(n_variables) / 8.0);
            for (auto i = 0; i < n_variables; ++i)
            {
                auto sum = 0.0;
                for (auto j = 0; j < n_variables; ++j)
                {
                    transformation_state_.second_transformation_matrix[i][j] = factor 
                        * transformation_state_.second_rotation.at(i).at(j);
                    sum += transformation_state_.second_rotation.at(j).at(i);
                }
                transformation_state_.transformation_base[i] = 0.5;
                optimum_.x[i] = sum / (2. * factor);
            }
        }
    };
}
