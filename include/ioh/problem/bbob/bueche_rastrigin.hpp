#pragma once

#include "rastrigin.hpp"

namespace ioh::problem::bbob
{
    //! BuecheRastrigin problem id 4
    class BuecheRastrigin final : public RastriginBase<BuecheRastrigin>
    {
    protected:
        //! Variables transformation method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, optimum_.x);
            oscillate(x);
            brs(x);
            return x;
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
            enforce_bounds(100);
            
            for (size_t i = 0; i < optimum_.x.size(); i += 2)
            {
                optimum_.x[i] = fabs(optimum_.x[i]);
            }
        }
    };
}
