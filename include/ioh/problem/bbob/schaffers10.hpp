#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{

    class Schaffers10 final: public BBOB, AutomaticFactoryRegistration<Schaffers10, RealProblem>
    
        {
         protected:
             std::vector<double> evaluate(std::vector<double>& x) override {}
         public:        
             Schaffers10(const int instance, const int n_variables) :                 
                BBOB(17, instance, n_variables, "Schaffers10") {}
        
        };
}