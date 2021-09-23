#pragma once
#include <numeric>
#include "wmodel_problem.hpp"

namespace ioh::problem::wmodel
{
    class WModelOneMax : public WModel
    {
    protected:
        int wmodel_evaluate(const std::vector<int> &x) override { return std::accumulate(x.begin(), x.end(), 0); }

    public:
        WModelOneMax(const int instance, const int n_variables, const double dummy_select_rate = 0.0,
                     const int epistasis_block_size = 0, const int neutrality_mu = 0, const int ruggedness_gamma = 0) :
            WModel(2, instance, n_variables, "WModelOneMax", dummy_select_rate, epistasis_block_size, neutrality_mu,
                   ruggedness_gamma)
        {
        }
    };
} // namespace ioh::problem::wmodel
