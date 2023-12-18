/**
 * @file dynamic_bin_val_ranking.hpp
 * @brief Declaration of DynamicBinValRanking class for dynamic binary value problems in IOH context.
 */

#pragma once

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

#include "ioh/problem/single.hpp"
#include "ioh/problem/transformation.hpp"

namespace ioh::problem
{
    /**
     * @class DynamicBinValRanking
     * @brief Represents dynamic binary value problems in Iterative Optimization Heuristics (IOH).
     *
     * It inherits from IntegerSingleObjective and includes additional functionalities for dynamic problems.
     * This class encapsulates the unique dynamic features such as changing optimum and comparison order over time.
     */
    class DynamicBinValRanking : public
        IntegerSingleObjective,
        AutomaticProblemRegistration<DynamicBinValRanking, DynamicBinValRanking>,
        AutomaticProblemRegistration<DynamicBinValRanking, IntegerSingleObjective>
    {
    public:

        int timestep; ///< Current timestep of the dynamic problem.
        std::mt19937 random_generator; ///< Random number generator.

        /**
         * Defines the comparison order for lexicographical sorting of bitstrings.
         * This vector determines the priority of indices for comparison.
         *
         * EXAMPLE:
         * If comparison_ordering is [1, 2, 0], index 1 has the highest priority in comparisons.
         * If we get two vectors [1, 0, 0] and [0, 0, 1], then
         * the second vector is larger because location 1 has the same value
         * and at location 2, 1 is greater than 0. We have never reached location 0, which was
             * to be compared last according to the comparison ordering [1, 2, 0].
         */
        std::vector<int> comparison_ordering;

        /**
         * @brief Constructs a new instance of DynamicBinValRanking.
         *
         * @param n_variables The dimension of the problem, representing the size of the search space and
         *                    indicating the number of variables in the problem.
         */
        DynamicBinValRanking(const int instance, const int n_variables) :
            IntegerSingleObjective
            (
                MetaData(10004, instance, "DynamicBinValRanking", n_variables, common::OptimizationType::MAX),
                Bounds<int>(n_variables, 0, 1)
            ),
            random_generator(instance),
            comparison_ordering(n_variables)
        {
            if (n_variables == 1) { return; }

            std::iota(comparison_ordering.begin(), comparison_ordering.end(), 0);

            this->optimum_.x = std::vector<int>(n_variables);
            std::uniform_int_distribution<> distrib(0, 1);
            for (int& value : this->optimum_.x) {
                value = distrib(this->random_generator);
            }
            this->optimum_.y = -1;
            this->timestep = 0;
        }

        void step()
        {
            std::shuffle(comparison_ordering.begin(), comparison_ordering.end(), this->random_generator);
            this->timestep += 1;
        }

        /**
         * @brief Sorts the given bitstrings lexicographically based on the ranking permutation.
         *
         * @param bitstrings The list of bitstrings to be sorted.
         * @return A sorted list of bitstrings.
         */
        std::vector<std::vector<int>> rank(const std::vector<std::vector<int>>& bitstrings)
        {
            // Copy the bitstrings to avoid modifying the original list
            std::vector<std::vector<int>> sorted_bitstrings = bitstrings;

            // Return True iff a > b.
            auto comparator = [this](const std::vector<int>& a, const std::vector<int>& b)
            {
                for (int index : this->comparison_ordering)
                {
                    if (a[index] != b[index])
                    {
                        if (this->optimum_.x[index] == 1)
                        {
                            return a[index] > b[index];
                        }
                        else
                        {
                            return a[index] < b[index];
                        }
                    }
                }
                return false;
            };

            std::sort(sorted_bitstrings.begin(), sorted_bitstrings.end(), comparator);

            return sorted_bitstrings;
        }

        const std::vector<int>& get_comparison_ordering() const
        {
            return comparison_ordering;
        }

        const int get_timestep() const
        {
            return timestep;
        }

    protected:

        /**
         * @brief Evaluates a given solution vector.
         *
         * This function is not used.
         *
         * @param x Solution vector to be evaluated.
         * @return Evaluation result as a double.
         */
        double evaluate(const std::vector<int> &x) override
        {
            return 0;
        }
    };
} // namespace ioh::problem
