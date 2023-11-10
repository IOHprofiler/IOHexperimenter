/**
 * @file dynamic_bin_val_powers_of_two.hpp
 * @brief Contains the declaration of the DynamicBinValPareto class, which represents dynamic binary value
 *        problems in the context of IOH.
 */

#pragma once

#include <algorithm>
#include <iostream>
#include <limits>
#include <math.h>
#include <random>
#include <vector>

#include "ioh/problem/single.hpp"
#include "ioh/problem/transformation.hpp"

namespace ioh::problem
{
    /**
     * @class DynamicBinValPareto
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
    class DynamicBinValPareto : public
        IntegerSingleObjective,
        AutomaticProblemRegistration<DynamicBinValPareto, DynamicBinValPareto>,
        AutomaticProblemRegistration<DynamicBinValPareto, IntegerSingleObjective>
    {
    public:

        int timestep;                   /**< The current timestep in the dynamic binary value problem scenario. */
        std::vector<unsigned long> weights;       /**< A vector of weights used in the evaluation of the problem. */
        std::default_random_engine random_generator;
        double pareto_upper_bound;
        double pareto_shape; // Alpha parameter for the Pareto distribution

        /**
         * @brief Constructs a new instance of DynamicBinValPareto.
         *
         * @param n_variables The dimension of the problem, representing the size of the search space and
         *                    indicating the number of variables in the problem.
         */
        DynamicBinValPareto(const int instance, const int n_variables) :
            IntegerSingleObjective
            (
                MetaData(10003, instance, "DynamicBinValPareto", n_variables, common::OptimizationType::MAX),
                Bounds<int>(n_variables, 0, 1)
            ),
            random_generator(instance),
            pareto_shape(0.1),
            pareto_upper_bound(static_cast<double>(std::numeric_limits<unsigned long>::max()) / static_cast<double>(n_variables))
        {
            if (n_variables == 1) { return; }

            this->timestep = 0;
            this->weights.resize(n_variables);

            std::uniform_real_distribution<double> distribution(0.0, 1.0);

            for(size_t i = 0; i < this->weights.size(); ++i) {
                double uniform_sample = distribution(this->random_generator);

                // Calculate the weight using the power-law distribution inversion formula
                // Truncate the distribution to prevent overflow when weights are summed
                auto pareto_distributed = std::min(std::pow(1.0 - uniform_sample, -1.0 / pareto_shape), pareto_upper_bound);
                this->weights[i] = static_cast<unsigned long>(pareto_distributed);
                if (this->weights[i] == 0)
                {
                    std::string error_message = "We sampled a 0 from a Pareto distribution at index " + std::to_string(i);
                    error_message += " with uniform_sample = " + std::to_string(uniform_sample);
                    error_message += ", pareto_distributed = " + std::to_string(pareto_distributed);
                    error_message += ", pareto_shape = " + std::to_string(pareto_shape);
                    error_message += ", pareto_upper_bound = " + std::to_string(pareto_upper_bound);
                    throw std::runtime_error(error_message);
                }
            }
        }

        int step()
        {
            this->timestep += 1;

            std::uniform_real_distribution<double> distribution(0.0, 1.0);

            for(size_t i = 0; i < this->weights.size(); ++i) {
                double uniform_sample = distribution(this->random_generator);

                // Calculate the weight using the power-law distribution inversion formula
                // Truncate the distribution to prevent overflow when weights are summed
                auto pareto_distributed = std::min(std::pow(1.0 - uniform_sample, -1.0 / pareto_shape), pareto_upper_bound);
                this->weights[i] = static_cast<unsigned long>(pareto_distributed);
                if (this->weights[i] == 0)
                {
                    std::string error_message = "We sampled a 0 from a Pareto distribution at index " + std::to_string(i);
                    error_message += " with uniform_sample = " + std::to_string(uniform_sample);
                    error_message += ", pareto_distributed = " + std::to_string(pareto_distributed);
                    error_message += ", pareto_shape = " + std::to_string(pareto_shape);
                    error_message += ", pareto_upper_bound = " + std::to_string(pareto_upper_bound);
                    throw std::runtime_error(error_message);
                }
            }

            return this->timestep;
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
            double value = 0;
            for(size_t i = 0; i < x.size(); ++i)
            {
                value += x[i] * this->weights[i];
            }

            return value;
        }
    };
} // namespace ioh::problem
