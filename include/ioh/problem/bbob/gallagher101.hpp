#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{

    class Gallagher101 final: public BBOB, AutomaticFactoryRegistration<Gallagher101, RealProblem>
    
        {
         protected:
             std::vector<double> evaluate(std::vector<double>& x) override {}
         public:        
             Gallagher101(const int instance, const int n_variables) :                 
                BBOB(21, instance, n_variables, "Gallagher101") {}
        
        };
}