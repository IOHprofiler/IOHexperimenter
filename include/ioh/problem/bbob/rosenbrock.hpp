#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{

    class Rosenbrock final: public BBOB, AutomaticFactoryRegistration<Rosenbrock, RealProblem>
    
        {
         protected:
             std::vector<double> evaluate(std::vector<double>& x) override {}
         public:        
             Rosenbrock(const int instance, const int n_variables) :                 
                BBOB(8, instance, n_variables, "Rosenbrock") {}
        
        };
}