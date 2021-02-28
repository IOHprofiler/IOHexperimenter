#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    class Gallagher21 final : public Gallagher<Gallagher21>
    {
    public:
        Gallagher21(const int instance, const int n_variables) :
            Gallagher(22, instance, n_variables, "Gallagher21", 21, 9.8, 4.9)
        {
        }
    };
}
