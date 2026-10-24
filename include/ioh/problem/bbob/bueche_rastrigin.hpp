#pragma once

#include "rastrigin.hpp"

namespace ioh::problem::bbob
{
    //! BuecheRastrigin problem id 4
    template<typename P = BBOB>
    class BuecheRastrigin final : public RastriginBase<P>
    {
    public:
        //! The id in the registry, also used by the constructor
        static constexpr int registry_id = 4;
        //! The name in the registry, also used by the constructor
        static constexpr const char *registry_name = "BuecheRastrigin";

    protected:
        //! Variables transformation method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, this->optimum_.x);
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
            RastriginBase<P>(registry_id, instance, n_variables, registry_name)
        {
            this->enforce_bounds(this->bounds_.weight * 100);
            for (size_t i = 0; i < this->optimum_.x.size(); i += 2)
            {
                this->optimum_.x[i] = fabs(this->optimum_.x[i]);
            }
        }
    };

    extern template class BuecheRastrigin<BBOB>;
    extern template class BuecheRastrigin<SBOX>;
}
