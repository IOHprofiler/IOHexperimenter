#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    //! Discuss function id 11
    template<typename P=BBOB>
    class Discus final : public P, BBOProblem<Discus>
    {
    protected:
        //! Evaluation method
        double evaluate(const std::vector<double> &z) override
        {
            double&& value = discus_func(z);
            return value;
        }
        //! Variables transformation method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, this->optimum_.x);
            affine(x, this->transformation_state_.transformation_matrix, this->transformation_state_.transformation_base);
            oscillate(x);
            return x;
        }

    public:
        /**
         * @brief Construct a new Discus object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem
         */
        Discus(const int instance, const int n_variables) :
            P(11, instance, n_variables, "Discus")
        {
        }
    };

    template class Discus<BBOB>;
}
