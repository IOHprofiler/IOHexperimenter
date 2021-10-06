#pragma once

#include "rastrigin.hpp"

namespace ioh::problem::bbob
{
    //! BuecheRastrigin problem id 4
    class BuecheRastrigin final : public RastriginBase<BuecheRastrigin>
    {
        const double penalty_factor_ = 100.0;

    protected:
        //! Variables transformation method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, objective_.x);
            oscillate(x);
            brs(x);
            return x;
        }
        
        //! Objectives transformation method
        double transform_objectives(const double y) override
        {
            using namespace transformation::objective;
            return penalize(state_.current.x, constraint_, penalty_factor_,shift(y, objective_.y));
        }

    public:
        /**
         * @brief Construct a new Bueche Rastrigin object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem
         */
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
