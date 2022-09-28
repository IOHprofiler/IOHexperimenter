#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    //! Linear Slope problem id 5
    class LinearSlope final : public BBOProblem<LinearSlope>
    {
    protected:
        //! Evaluation method
        double evaluate(const std::vector<double> &x) override
        {
            auto result = 0.0;
            for (auto i = 0; i < meta_data_.n_variables; ++i)
                result += 5.0 * fabs(transformation_state_.conditions.at(i)) - transformation_state_.conditions.at(i) *
                (
                    x.at(i) * optimum_.x.at(i) < 25.0 ? x.at(i) : optimum_.x.at(i)
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
            BBOProblem(5, instance, n_variables, "LinearSlope")
        {
            static const auto base = sqrt(100.0);
            for (auto i = 0; i < meta_data_.n_variables; ++i)
                if (optimum_.x.at(i) < 0.0)
                {
                    optimum_.x[i] = -5; // constraints_.lb.at(0);
                    transformation_state_.conditions[i] = -pow(base, transformation_state_.exponents.at(i));
                }
                else
                {
                    optimum_.x[i] = 5; //constraints_.ub.at(0);
                    transformation_state_.conditions[i] = pow(base, transformation_state_.exponents.at(i));
                }
        }
    };
}
