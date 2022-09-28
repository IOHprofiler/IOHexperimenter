#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    //! Attractive Sector problem id = 2
    class AttractiveSector final : public BBOProblem<AttractiveSector>
    {
    protected:

        //! Evaluation method
        double evaluate(const std::vector<double> &x) override
        {
            using namespace transformation::objective;
            auto result =  0.0 ;
            for (auto i = 0; i < meta_data_.n_variables; ++i)
                result += x.at(i) * x.at(i) * (1. + 9999.0 * (optimum_.x.at(i) * x.at(i) > 0.0));
            return pow(oscillate(result), .9);
        }
        
        //! Variables transformation method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, optimum_.x);
            affine(x, transformation_state_.second_transformation_matrix, transformation_state_.transformation_base);
            return x;
        }

    public:
        /**
         * @brief Construct a new Attractive Sector object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem
         */
        AttractiveSector(const int instance, const int n_variables) :
            BBOProblem(6, instance, n_variables, "AttractiveSector")
        {
        }
    };
}
