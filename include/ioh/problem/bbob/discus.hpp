#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{

    class Discus final: public BBOB, AutomaticFactoryRegistration<Discus, RealProblem>
    
        {
         protected:
             std::vector<double> evaluate(std::vector<double>& x) override {}
         public:        
             Discus(const int instance, const int n_variables) :                 
                BBOB(11, instance, n_variables, "Discus") {}
        
        };
}