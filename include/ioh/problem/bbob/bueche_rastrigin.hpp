#pragma once

#include "rastrigin.hpp"

namespace ioh::problem::bbob
{
    class BuecheRastrigin final : public RastriginBase<BuecheRastrigin>
    {
        const double penalty_factor_ = 100.0;

    protected:
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, objective_.x);
            oscillate(x);
            brs(x);
            return x;
        }

        double transform_objectives(const double y) override
        {
            using namespace transformation::objective;
            return penalize(state_.current.x, constraint_, penalty_factor_,shift(y, objective_.y));
        }

    public:
        BuecheRastrigin(const int instance, const int n_variables) :
            RastriginBase(4, instance, n_variables, "BuecheRastrigin")
        {
            for (size_t i = 0; i < objective_.x.size(); i += 2)
            {
                objective_.x[i] = fabs(objective_.x[i]);
            }
        }
    };
}
