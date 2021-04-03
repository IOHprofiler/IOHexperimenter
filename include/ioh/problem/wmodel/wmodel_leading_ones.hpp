#pragma once
#include "wmodel_problem.hpp"

namespace ioh::problem::wmodel
{
    class WModelLeadingOnes final : public WModel
    {
    protected:
        int wmodel_evaluate(const std::vector<int> &x) override
        {
            const auto query = std::find(x.begin(), x.end(), 0);
            return static_cast<int>(query == x.end() ? x.size() : std::distance(x.begin(), query));
        }

    public:
        WModelLeadingOnes(const int instance, const int n_variables,
                          const double dummy_para, const int epistasis_para, const int neutrality_para,
                          const int ruggedness_para) :
            WModel(2, instance, n_variables, "WModelLeadingOnes",
                   dummy_para, epistasis_para, neutrality_para,
                   ruggedness_para)
        {
        }
    };
}
