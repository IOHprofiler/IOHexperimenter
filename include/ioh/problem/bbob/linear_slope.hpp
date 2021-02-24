#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{

    class LinearSlope final: public BBOB, AutomaticFactoryRegistration<LinearSlope, RealProblem>
    
        {
         protected:
             std::vector<double> evaluate(std::vector<double>& x) override {}
         public:        
             LinearSlope(const int instance, const int n_variables) :                 
                BBOB(5, instance, n_variables, "LinearSlope") {}
        
        };
}