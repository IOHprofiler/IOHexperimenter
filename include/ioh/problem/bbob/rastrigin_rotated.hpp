#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{

    class RastriginRotated final: public BBOB, AutomaticFactoryRegistration<RastriginRotated, RealProblem>
    
        {
         protected:
             std::vector<double> evaluate(std::vector<double>& x) override {}
         public:        
             RastriginRotated(const int instance, const int n_variables) :                 
                BBOB(15, instance, n_variables, "RastriginRotated") {}
        
        };
}