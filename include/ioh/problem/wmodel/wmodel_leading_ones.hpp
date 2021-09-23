#pragma once
#include <algorithm>
#include "wmodel_problem.hpp"

namespace ioh::problem::wmodel
{
    class WModelLeadingOnes : public WModel
    {
    protected:
        int wmodel_evaluate(const std::vector<int> &x) override
        {
            const auto query = std::find(x.begin(), x.end(), 0);
            return static_cast<int>(query == x.end() ? x.size() : std::distance(x.begin(), query));
        }

    public:
        WModelLeadingOnes(const int instance, const int n_variables, const double dummy_select_rate = 0.0,
                          const int epistasis_block_size =0, const int neutrality_mu=0, const int ruggedness_gamma=0) :
            WModel(2, instance, n_variables, "WModelLeadingOnes", dummy_select_rate, epistasis_block_size,
                   neutrality_mu, ruggedness_gamma)
        {
        }
    };
} // namespace ioh::problem::wmodel
