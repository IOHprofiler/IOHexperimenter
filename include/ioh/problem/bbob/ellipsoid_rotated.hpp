#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{

    class EllipsoidRotated final: public BBOB, AutomaticFactoryRegistration<EllipsoidRotated, RealProblem>
    
        {
         protected:
             std::vector<double> evaluate(std::vector<double>& x) override {}
         public:        
             EllipsoidRotated(const int instance, const int n_variables) :                 
                BBOB(10, instance, n_variables, "EllipsoidRotated") {}
        
        };
}