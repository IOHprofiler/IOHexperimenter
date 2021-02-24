#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{

    class BuecheRastrigin final: public BBOB, AutomaticFactoryRegistration<BuecheRastrigin, RealProblem>
    
        {
         protected:
             std::vector<double> evaluate(std::vector<double>& x) override {}
         public:        
             BuecheRastrigin(const int instance, const int n_variables) :                 
                BBOB(4, instance, n_variables, "BuecheRastrigin") {}
        
        };
}