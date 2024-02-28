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
     *        DynamicBinValPareto: takes a value from (1 - U) ** (-10) where U is uniformly distributed from [0, 1] for each weight at each timestep

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

        double pareto_shape;                            // Alpha parameter for the Pareto distribution
        double pareto_upper_bound;
        int timestep;                                   /**< The current timestep in the dynamic binary value problem scenario. */
        std::default_random_engine random_generator;
        std::vector<double> weights;             /**< A vector of weights used in the evaluation of the problem. */
        std::vector<int> transformed_x;

        /**
         * @brief Constructs a new instance of DynamicBinValPareto.
         *
         * @param n_variables The dimension of the problem, representing the size of the search space and
         *                    indicating the number of variables in the problem.
         */
        DynamicBinValPareto(const int instance, const int n_variables) :
            IntegerSingleObjective(
                MetaData(
                    10003,
                    instance,
                    "DynamicBinValPareto",
                    n_variables,
                    common::OptimizationType::MAX
                ),
                Bounds<int>(
                    n_variables,
                    0,
                    1
                )
            ),
            pareto_shape(0.1),
            pareto_upper_bound(
                static_cast<double>(std::numeric_limits<double>::max()) / static_cast<double>(n_variables)
            ),
            timestep(0),
            random_generator(instance)
        {
            if (n_variables == 1) { return; }

            this->weights.resize(n_variables);

            std::uniform_real_distribution<double> distribution(0.0, 0.75);

            for(size_t i = 0; i < this->weights.size(); ++i)
            {
                double uniform_sample = distribution(this->random_generator);

                // Calculate the weight using the power-law distribution inversion formula
                // Truncate the distribution to prevent overflow when weights are summed
                auto pareto_distributed = std::min(std::pow(1.0 - uniform_sample, -1.0 / pareto_shape), pareto_upper_bound);
                this->weights[i] = static_cast<double>(pareto_distributed);
            }

            this->transformed_x = std::vector<int>(this->weights.size(), 1);
            this->optimum_.y = transform_objectives(0);
            transform_variables(this->transformed_x);
            this->optimum_.x = this->transformed_x;
        }

        int step()
        {
            this->timestep += 1;

            std::uniform_real_distribution<double> distribution(0.0, 0.75);

            for(size_t i = 0; i < this->weights.size(); ++i)
            {
                double uniform_sample = distribution(this->random_generator);
                auto pareto_distributed = std::min(std::pow(1.0 - uniform_sample, -1.0 / pareto_shape), pareto_upper_bound);
                this->weights[i] = static_cast<double>(pareto_distributed);
            }

            this->transformed_x = std::vector<int>(this->weights.size(), 1);
            this->optimum_.y = transform_objectives(0);
            transform_variables(this->transformed_x);
            this->optimum_.x = this->transformed_x;

            return this->timestep;
        }

        const double get_pareto_shape() const
        {
            return pareto_shape;
        }

        const double get_pareto_upper_bound() const
        {
            return pareto_upper_bound;
        }

    protected:

        double evaluate(const std::vector<int> &x) override
        {
            return std::accumulate(x.begin(), x.end(), 0.0);
        }

        std::vector<int> transform_variables(std::vector<int> x) override
        {
            transformation::variables::random_flip(x, this->meta_data_.instance);
            this->transformed_x = x;
            return x;
        }

        double transform_objectives(const double y) override
        {
            double value = 0.0;
            for(size_t i = 0; i < this->transformed_x.size(); ++i)
            {
                value += this->transformed_x[i] * this->weights[i];
            }
            return value;
        }
    };
} // namespace ioh::problem
