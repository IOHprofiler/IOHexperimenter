#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{

    class Schwefel final: public BBOB, AutomaticFactoryRegistration<Schwefel, RealProblem>
    
        {
         protected:
             std::vector<double> evaluate(std::vector<double>& x) override {}
         public:        
             Schwefel(const int instance, const int n_variables) :                 
                BBOB(20, instance, n_variables, "Schwefel") {}
        
        };
}