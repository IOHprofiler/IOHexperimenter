#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    //! Linear Slope problem id 5
    template<typename P=BBOB>
    class LinearSlope final : public P, BBOProblem<LinearSlope>
    {
    protected:
        //! Evaluation method
        double evaluate(const std::vector<double> &x) override
        {
            auto result = 0.0;
            for (auto i = 0; i < this->meta_data_.n_variables; ++i)
                result += 5.0 * fabs(this->transformation_state_.conditions[i]) - this->transformation_state_.conditions[i] *
                (
                    x[i] * this->optimum_.x[i] < 25.0 ? x[i] : this->optimum_.x[i]
                );
            return result;
        }

    public:
        /**
         * @brief Construct a new Linear Slope object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem
         */
        LinearSlope(const int instance, const int n_variables) :
            P(5, instance, n_variables, "LinearSlope")
        {
            static const auto base = sqrt(100.0);
            for (auto i = 0; i < this->meta_data_.n_variables; ++i)
                if (this->optimum_.x[i] < 0.0)
                {
                    this->optimum_.x[i] = -5; // constraints_.lb.at(0);
                    this->transformation_state_.conditions[i] = -pow(base, this->transformation_state_.exponents[i]);
                }
                else
                {
                    this->optimum_.x[i] = 5; //constraints_.ub.at(0);
                    this->transformation_state_.conditions[i] = pow(base, this->transformation_state_.exponents[i]);
                }
        }
    };

    template class LinearSlope<BBOB>; 
}
