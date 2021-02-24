#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{

    class Katsuura final: public BBOB, AutomaticFactoryRegistration<Katsuura, RealProblem>
    
        {
         protected:
             std::vector<double> evaluate(std::vector<double>& x) override {}
         public:        
             Katsuura(const int instance, const int n_variables) :                 
                BBOB(23, instance, n_variables, "Katsuura") {}
        
        };
}