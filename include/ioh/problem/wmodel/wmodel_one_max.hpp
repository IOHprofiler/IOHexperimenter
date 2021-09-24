#pragma once
#include <numeric>
#include "wmodel_problem.hpp"

//! WModel namespace
namespace ioh::problem::wmodel
{
    //! WModelOneMax
    class WModelOneMax : public WModel
    {
    protected:
        //! Evaluation method for WModel functions
        int wmodel_evaluate(const std::vector<int> &x) override { return std::accumulate(x.begin(), x.end(), 0); }

    public:
        /**
         * @brief Construct a new WModelOneMax object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem
         * @param dummy_select_rate select rate
         * @param epistasis_block_size block size
         * @param neutrality_mu neutrality parameter
         * @param ruggedness_gamma ruggedness parameter
         */
        WModelOneMax(const int instance, const int n_variables, const double dummy_select_rate = 0.0,
                     const int epistasis_block_size = 0, const int neutrality_mu = 0, const int ruggedness_gamma = 0) :
            WModel(2, instance, n_variables, "WModelOneMax", dummy_select_rate, epistasis_block_size, neutrality_mu,
                   ruggedness_gamma)
        {
        }
    };
} // namespace ioh::problem::wmodel
