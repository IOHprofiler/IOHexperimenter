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
        double evaluate(const std::vector<double> &x) override
        {
            static const auto condition = 1.0e6;
            auto result = condition * x.at(0) * x.at(0);
            for (auto i = 1; i < this->meta_data_.n_variables; ++i)
                result += x[i] * x[i];
            return result;
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
