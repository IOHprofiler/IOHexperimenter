#pragma once
#include <algorithm>
#include "wmodel_problem.hpp"

namespace ioh::problem::wmodel
{
    //! WModelLeadingOnes
    class WModelLeadingOnes : public WModel
    {
    protected:
        //! Evaluation method for WModel functions
        int wmodel_evaluate(const std::vector<int> &x) override
        {
            const auto query = std::find(x.begin(), x.end(), 0);
            return static_cast<int>(query == x.end() ? x.size() : std::distance(x.begin(), query));
        }

    public:
        /**
         * @brief Construct a new WModelLeadingOnes object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem
         * @param dummy_select_rate select rate
         * @param epistasis_block_size block size
         * @param neutrality_mu neutrality parameter
         * @param ruggedness_gamma ruggedness parameter
         */
        WModelLeadingOnes(const int instance, const int n_variables, const double dummy_select_rate = 0.0,
                          const int epistasis_block_size =0, const int neutrality_mu=0, const int ruggedness_gamma=0) :
            WModel(2, instance, n_variables, "WModelLeadingOnes", dummy_select_rate, epistasis_block_size,
                   neutrality_mu, ruggedness_gamma)
        {
        }
    };
} // namespace ioh::problem::wmodel
