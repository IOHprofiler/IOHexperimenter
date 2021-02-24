#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{

    class Schaffers1000 final: public BBOB, AutomaticFactoryRegistration<Schaffers1000, RealProblem>
    
        {
         protected:
             std::vector<double> evaluate(std::vector<double>& x) override {}
         public:        
             Schaffers1000(const int instance, const int n_variables) :                 
                BBOB(18, instance, n_variables, "Schaffers1000") {}
        
        };
}