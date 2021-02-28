#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{

    class Gallagher21 final: public BBOB<Gallagher21>
    
        {
         protected:
             std::vector<double> evaluate(std::vector<double>& x) override {}
         public:        
             Gallagher21(const int instance, const int n_variables) :                 
                BBOB(22, instance, n_variables, "Gallagher21") {}
        
        };
}