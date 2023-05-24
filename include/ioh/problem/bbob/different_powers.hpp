#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    //! Different powers problem id 14
    template<typename P=BBOB>
    class DifferentPowers final : public P, BBOProblem<DifferentPowers>
    {
    protected:
        //! Evaluation method
        double evaluate(const std::vector<double> &x) override
        {
            auto sum = 0.0;
            for (auto i = 0; i < this->meta_data_.n_variables; ++i)
                sum += pow(fabs(x[i]), this->transformation_state_.exponents[i]);
            return sqrt(sum);
        }
        //! Variables transformation method                         
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, this->optimum_.x);
            affine(x, this->transformation_state_.transformation_matrix, this->transformation_state_.transformation_base);
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
            P(14, instance, n_variables, "DifferentPowers")
        {
            for (auto i = 0; i < this->meta_data_.n_variables; ++i)
                this->transformation_state_.exponents[i] = 2.0 + 4.0 * this->transformation_state_.exponents[i];
        }
    };
    template class DifferentPowers<BBOB>;
}
