/**
 * @file dynamic_bin_val.hpp
 * @brief Contains the declaration of the DynamicBinVal class, which represents dynamic binary value
 *        problems in the context of IOH.
 */

#pragma once

#include "ioh/problem/single.hpp"
#include "ioh/problem/transformation.hpp"

namespace ioh::problem
{
    /**
     * @class DynamicBinVal
     * @brief This class serves to represent dynamic binary value problems within the context of Iterative
     *        Optimization Heuristics (IOH).
     *
     *        Inheriting functionalities from the IntegerSingleObjective, it also integrates functionalities
     *        for automatic registration of the problem type into various data structures. This facilitates
     *        easier management and retrieval of problem instances, while encapsulating characteristics and
     *        behaviours specific to dynamic binary value problems. It holds vital data members such as
     *        timestep and weights, which are crucial in depicting the dynamic aspects and unique features
     *        of these problem instances.
     */
    class DynamicBinVal : public
        IntegerSingleObjective,
        AutomaticProblemRegistration<DynamicBinVal, DynamicBinVal>,
        AutomaticProblemRegistration<DynamicBinVal, IntegerSingleObjective>
    {
    public:

        int timestep; /**< The current timestep in the dynamic binary value problem scenario. */
        std::vector<int> weights; /**< A vector of weights used in the evaluation of the problem. */

        /**
         * @brief Constructs a new instance of DynamicBinVal.
         *
         * @param n_variables The dimension of the problem, representing the size of the search space and
         *                    indicating the number of variables in the problem.
         */
        DynamicBinVal(const int instance, const int n_variables) : IntegerSingleObjective
        (
            MetaData(10001, 1, "DynamicBinVal", n_variables, common::OptimizationType::MAX),
            Bounds<int>(n_variables, 0, 1)
        )
        {
            if (n_variables == 1) { return; }

            this->timestep = 0;

            // Initialize the weights vector with powers of two
            this->weights.resize(n_variables);
            this->weights[0] = 1;
            for(int i = 1; i < n_variables; ++i)
            {
                this->weights[i] = this->weights[i-1] * 2;
            }
        }

    protected:

        /**
         * @brief Evaluates the problem instance using the given input vector.
         *
         * @param x The input vector which represents a potential solution to the problem.
         * @return The evaluation result as a double value.
         */
        double evaluate(const std::vector<int> &x) override
        {
            int value = 0; // Initialize value to 0 to calculate it based on the weights and x
            for(size_t i = 0; i < x.size(); ++i)
            {
                value += x[i] * this->weights[i];
            }

            return static_cast<double>(value);
        }
    };
} // namespace ioh::problem
