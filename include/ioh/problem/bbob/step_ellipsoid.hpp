#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{

    class StepEllipsoid final: public BBOB, AutomaticFactoryRegistration<StepEllipsoid, RealProblem>
    
        {
         protected:
             std::vector<double> evaluate(std::vector<double>& x) override {}
         public:        
             StepEllipsoid(const int instance, const int n_variables) :                 
                BBOB(7, instance, n_variables, "StepEllipsoid") {}
        
        };
}