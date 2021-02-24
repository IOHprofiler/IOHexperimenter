#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{

    class RosenbrockRotated final: public BBOB, AutomaticFactoryRegistration<RosenbrockRotated, RealProblem>
    
        {
         protected:
             std::vector<double> evaluate(std::vector<double>& x) override {}
         public:        
             RosenbrockRotated(const int instance, const int n_variables) :                 
                BBOB(9, instance, n_variables, "RosenbrockRotated") {}
        
        };
}