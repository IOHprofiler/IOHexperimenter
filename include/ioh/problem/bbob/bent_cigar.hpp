#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    //! Bent Cigar problem id = 12
    class BentCigar final : public BBOProblem<BentCigar>
    {
    protected:
        //! Evaluation method
        double evaluate(const std::vector<double> &x) override
        {
            static const auto condition = 1.0e6;
            auto result = x.at(0) * x.at(0);
            for (auto i = 1; i < meta_data_.n_variables; ++i)
                result += condition * x.at(i) * x.at(i);
            return result;
        }

        //! Variables transformation method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, optimum_.x);
            affine(x, transformation_state_.transformation_matrix, transformation_state_.transformation_base);
            asymmetric(x, 0.5);
            affine(x, transformation_state_.transformation_matrix, transformation_state_.transformation_base);
            return x;
        }

    public:
        /**
         * @brief Construct a new Bent Cigar object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem
         */
        BentCigar(const int instance, const int n_variables) :
            BBOProblem(12, instance, n_variables, "BentCigar")
        {
        }
    };
}
