#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    //! Different powers problem id 14
    class DifferentPowers final : public BBOProblem<DifferentPowers>
    {
    protected:
        //! Evaluation method
        double evaluate(const std::vector<double> &x) override
        {
            auto sum = 0.0;
            for (auto i = 0; i < meta_data_.n_variables; ++i)
                sum += pow(fabs(x.at(i)), transformation_state_.exponents.at(i));
            return sqrt(sum);
        }
        //! Variables transformation method                         
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, optimum_.x);
            affine(x, transformation_state_.transformation_matrix, transformation_state_.transformation_base);
            return x;
        }

    public:
        /**
         * @brief Construct a new Different Powers object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem
         */
        DifferentPowers(const int instance, const int n_variables) :
            BBOProblem(14, instance, n_variables, "DifferentPowers")
        {
            for (auto i = 0; i < meta_data_.n_variables; ++i)
                transformation_state_.exponents[i] = 2.0 + 4.0 * transformation_state_.exponents.at(i);
        }
    };
}
