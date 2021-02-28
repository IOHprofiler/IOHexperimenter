#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    class LinearSlope final : public BBOBBase<LinearSlope>
    {
    protected:
        std::vector<double> evaluate(std::vector<double> &x) override
        {
            auto result = 0.0;
            for (auto i = 0; i < meta_data_.n_variables; ++i)
                result += 5.0 * fabs(transformation_state_.conditions.at(i)) - transformation_state_.conditions.at(i) *
                (
                    x.at(i) * meta_data_.objective.x.at(i) < 25.0 ? x.at(i) : meta_data_.objective.x.at(i)
                );
            return {result};
        }

    public:
        LinearSlope(const int instance, const int n_variables) :
            BBOBBase(5, instance, n_variables, "LinearSlope")
        {
            static const auto base = sqrt(100.0);
            for (auto i = 0; i < meta_data_.n_variables; ++i)
                if (meta_data_.objective.x.at(i) < 0.0)
                {
                    meta_data_.objective.x[i] = constraint_.lb.at(0);
                    transformation_state_.conditions[i] = -pow(base, transformation_state_.exponents.at(i));
                }
                else
                {
                    meta_data_.objective.x[i] = constraint_.ub.at(0);
                    transformation_state_.conditions[i] = pow(base, transformation_state_.exponents.at(i));
                }
        }
    };
}
