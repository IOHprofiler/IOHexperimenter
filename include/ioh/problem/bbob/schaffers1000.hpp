#pragma once

#include "schaffers10.hpp"

namespace ioh::problem::bbob
{
    class Schaffers1000 final: public Schaffers<Schaffers1000>
    {
    public:
        Schaffers1000(const int instance, const int n_variables) :
            Schaffers(18, instance, n_variables,  "Schaffers1000", 1000.0)
        {
        }
    };



}
