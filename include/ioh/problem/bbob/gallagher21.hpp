#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    //! Gallagher 21 problem id 22
    template<typename P = BBOB>
    class Gallagher21 final : public Gallagher<P>, BBOProblem<Gallagher21>
    {
    public:
        /**
         * @brief Construct a new Gallagher 2 1 object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem
         */
        Gallagher21(const int instance, const int n_variables) :
            Gallagher<P>(22, instance, n_variables, "Gallagher21", 21, 9.8, 4.9, 1000.)
        {
        }
    };
    template class Gallagher21<BBOB>;
}
