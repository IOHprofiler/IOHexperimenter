#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{

    class AttractiveSector final: public BBOB, AutomaticFactoryRegistration<AttractiveSector, RealProblem>
    
        {
         protected:
             std::vector<double> evaluate(std::vector<double>& x) override {}
         public:        
             AttractiveSector(const int instance, const int n_variables) :                 
                BBOB(6, instance, n_variables, "AttractiveSector") {}
        
        };
}