#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    //! Bent Cigar problem id = 12
    template <typename P=BBOB>
    class BentCigar final : public P, BBOProblem<BentCigar>
    {
    protected:
        //! Evaluation method
        double evaluate(const std::vector<double> &x) override
        {
            static constexpr auto condition = 1.0e6;
            auto result = x.at(0) * x.at(0);
            for (auto i = 1; i < this->meta_data_.n_variables; ++i)
                result += condition * x[i] * x[i];
            return result;
        }

        //! Variables transformation method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, this->optimum_.x);
            affine(x, this->transformation_state_.transformation_matrix,
                   this->transformation_state_.transformation_base);
            asymmetric(x, 0.5);
            affine(x, this->transformation_state_.transformation_matrix,
                   this->transformation_state_.transformation_base);
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
            P(12, instance, n_variables, "BentCigar")
        {
        }
    };

    template class BentCigar<BBOB>;
}
