#pragma once

#include "schaffers10.hpp"

namespace ioh::problem::bbob
{
    //! Shaffers 1000 problem id 18
    template<typename P=BBOB>
    class Schaffers1000 final : public Schaffers<P>
    {
    public:
        //! The id in the registry, also used by the constructor
        static constexpr int registry_id = 18;
        //! The name in the registry, also used by the constructor
        static constexpr const char *registry_name = "Schaffers1000";

    public:
        /**
         * @brief Construct a new Schaffers 1 0 0 0 object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem 
         */
        Schaffers1000(const int instance, const int n_variables) :
            Schaffers<P>(registry_id, instance, n_variables, registry_name, 1000.0)
        {
        }
    };
    extern template class Schaffers1000<BBOB>;
    extern template class Schaffers1000<SBOX>;
}
