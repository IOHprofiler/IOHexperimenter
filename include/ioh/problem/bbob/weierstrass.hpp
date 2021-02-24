#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{

    class Weierstrass final: public BBOB, AutomaticFactoryRegistration<Weierstrass, RealProblem>
    
        {
         protected:
             std::vector<double> evaluate(std::vector<double>& x) override {}
         public:        
             Weierstrass(const int instance, const int n_variables) :                 
                BBOB(16, instance, n_variables, "Weierstrass") {}
        
        };
}