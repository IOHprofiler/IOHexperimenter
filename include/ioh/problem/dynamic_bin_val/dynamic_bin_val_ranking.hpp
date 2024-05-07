/**
 * @file dynamic_bin_val_ranking.hpp
 * @brief Declaration of DynamicBinValRanking class for dynamic binary value problems in IOH context.
 */

#pragma once

#include "dynamic_bin_val.hpp"

namespace ioh::problem
{
    template <typename RandomGenerator>
    void portable_shuffle(std::vector<int>& array, RandomGenerator& generator) {
      for (size_t i = array.size() - 1; i > 0; --i) {
        std::uniform_int_distribution<size_t> distribution(0, i);
        size_t j = distribution(generator);
        std::swap(array[i], array[j]);
      }
    }

    /**
     * @class DynamicBinValRanking
     * @brief Represents dynamic binary value problems in Iterative Optimization Heuristics (IOH).
     *
     * It inherits from IntegerSingleObjective and includes additional functionalities for dynamic problems.
     * This class encapsulates the unique dynamic features such as changing optimum and comparison order over time.
     */
    class DynamicBinValRanking final : public DynamicBinValProblem<DynamicBinValRanking>
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
            DynamicBinValProblem(
                10'004,
                instance,
                n_variables,
                "DynamicBinValRanking"
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
            this->optimum_.y = this->evaluate(this->optimum_.x);
            this->timestep = 0;
        }

        int step() override
        {
            // Using std::shuffle results in different behaviour on Windows versus on Ubuntu.
            portable_shuffle(comparison_ordering, this->random_generator);
            this->timestep += 1;
            return this->timestep;
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

            // This is to update the evaluations count and best solution seen so far.
            for (auto& bitstring : bitstrings) {
                (*this)(bitstring);
            }

            return sorted_bitstrings;
        }

        /**
         * @brief Returns the indices of the given bitstrings sorted lexicographically based on the ranking permutation.
         *
         * This function generates a list of indices representing the order in which the bitstrings would be sorted.
         * It uses the same comparison criteria as the rank function, but instead of sorting the bitstrings themselves,
         * it sorts their indices. This is useful for understanding the original positions of the bitstrings in the sorted order.
         *
         * @param bitstrings The list of bitstrings for which the sorted indices are to be obtained.
         * @return A vector of indices indicating the sorted order of the bitstrings.
         */
        std::vector<int> rank_indices(const std::vector<std::vector<int>>& bitstrings)
        {
            // Create an index list from 0 to n-1
            std::vector<int> indices(bitstrings.size());
            std::iota(indices.begin(), indices.end(), 0);

            // Comparator using the comparison_ordering and optimum_.x from 'this' object
            auto comparator = [this, &bitstrings](int i, int j) {
                for (int index : this->comparison_ordering) {
                    if (bitstrings[i][index] != bitstrings[j][index]) {
                        return this->optimum_.x[index] == 1 ? bitstrings[i][index] > bitstrings[j][index] : bitstrings[i][index] < bitstrings[j][index];
                    }
                }
                return false;
            };

            // Sort the indices based on the comparator
            std::sort(indices.begin(), indices.end(), comparator);

            // This is to update the evaluations count and best solution seen so far.
            for (auto& bitstring : bitstrings) {
                (*this)(bitstring);
            }

            return indices;
        }

        const std::vector<int>& get_comparison_ordering() const
        {
            return comparison_ordering;
        }

        int get_timestep() const
        {
            return timestep;
        }

    protected:

        /**
         * @brief Evaluates the fitness of a bitstring for the OneMax problem.
         *
         * The OneMax problem aims to maximize the number of 1s in a binary string. This function calculates
         * the fitness of a given binary vector by summing its elements. Since the vector contains only 0s and 1s,
         * this sum is equivalent to counting the number of 1s in the vector, which is the desired fitness measure
         * for the OneMax problem.
         *
         * @param x The binary vector to be evaluated.
         * @return The fitness score, which is the sum of elements in the vector (number of 1s).
         */
        double evaluate(const std::vector<int> &x) override
        {
          // XOR the elements of the vector x with optimum_.x, then sum them
          int sum = 0;
          for (size_t i = 0; i < x.size(); ++i) {
            sum += 1 - (x[i] ^ this->optimum_.x[i]);
          }

          // The evaluation function returns the sum
          return static_cast<double>(sum);
        }

    };
} // namespace ioh::problem
