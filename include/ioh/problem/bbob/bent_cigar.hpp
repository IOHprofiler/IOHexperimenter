#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{

    class BentCigar final: public BBOB, AutomaticFactoryRegistration<BentCigar, RealProblem>
    
        {
         protected:
             std::vector<double> evaluate(std::vector<double>& x) override {}
         public:        
             BentCigar(const int instance, const int n_variables) :                 
                BBOB(12, instance, n_variables, "BentCigar") {}
        
        };
}