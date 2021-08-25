#pragma once
#include "wmodel_problem.hpp"
#include <numeric>

namespace ioh::problem::wmodel
{
    class WModelOneMax : public WModel
    {
    protected:
        int wmodel_evaluate(const std::vector<int> &x) override
        {
            return std::accumulate(x.begin(), x.end(), 0);
        }

    public:
        WModelOneMax(const int instance, const int n_variables,
                     const double dummy_para, const int epistasis_para, const int neutrality_para,
                     const int ruggedness_para) :
            WModel(1, instance, n_variables, "WModelOneMax", dummy_para, epistasis_para, neutrality_para,
                   ruggedness_para)
        {
            
        }
    };
}
