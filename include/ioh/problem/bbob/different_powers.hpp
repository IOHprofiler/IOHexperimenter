#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{

    class DifferentPowers final: public BBOB, AutomaticFactoryRegistration<DifferentPowers, RealProblem>
    
        {
         protected:
             std::vector<double> evaluate(std::vector<double>& x) override {}
         public:        
             DifferentPowers(const int instance, const int n_variables) :                 
                BBOB(14, instance, n_variables, "DifferentPowers") {}
        
        };
}