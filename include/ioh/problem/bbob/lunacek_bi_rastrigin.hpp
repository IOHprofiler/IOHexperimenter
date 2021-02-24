#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{

    class LunacekBiRastrigin final: public BBOB, AutomaticFactoryRegistration<LunacekBiRastrigin, RealProblem>
    
        {
         protected:
             std::vector<double> evaluate(std::vector<double>& x) override {}
         public:        
             LunacekBiRastrigin(const int instance, const int n_variables) :                 
                BBOB(24, instance, n_variables, "LunacekBiRastrigin") {}
        
        };
}