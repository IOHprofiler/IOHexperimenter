#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{

    class SharpRidge final: public BBOB, AutomaticFactoryRegistration<SharpRidge, RealProblem>
    
        {
         protected:
             std::vector<double> evaluate(std::vector<double>& x) override {}
         public:        
             SharpRidge(const int instance, const int n_variables) :                 
                BBOB(13, instance, n_variables, "SharpRidge") {}
        
        };
}