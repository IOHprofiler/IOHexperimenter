#pragma once
/**
 * @file dynamic_bin_val_ranking.hpp
 * @brief Declaration of DynamicBinValRanking class for dynamic binary value problems in IOH context.
 */

#include "dynamic_bin_val.hpp"

namespace ioh::problem::dynamic_bin_val
{
    // Redefined portable_shuffle using the uniform function for generating random indices
    template <typename RandomGenerator>
    void portable_shuffle(std::vector<int> &array, RandomGenerator &generator)
    {
        size_t n = array.size();
        if (n > 1)
        {
            // Generate random numbers between 0 and n-1 using the uniform function
            auto rand_values = ioh::common::random::pbo::uniform(n - 1, generator(), 0, static_cast<double>(n - 1));

            for (size_t i = n - 1; i > 0; --i)
            {
                // Convert generated double to an index within the valid range
                size_t j = static_cast<size_t>(rand_values[i - 1]);
                std::swap(array[i], array[j]);
            }
        }
    }

    /**
     * @class Ranking
     * @brief Represents dynamic binary value problems in Iterative Optimization Heuristics (IOH).
     *
     * It inherits from IntegerSingleObjective and includes additional functionalities for dynamic problems.
     * This class encapsulates the unique dynamic features such as changing optimum and comparison order over time.
     */
    class Ranking final : public DynamicBinValProblem<Ranking>
    {
    public:
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
         * @param instance random instance
         * @param n_variables The dimension of the problem, representing the size of the search space and
         *                    indicating the number of variables in the problem.
         */
        Ranking(const int instance, const int n_variables) :
            DynamicBinValProblem(10'004, instance, n_variables, "DynamicBinValRanking"),
            comparison_ordering(n_variables)
        {
            std::iota(comparison_ordering.begin(), comparison_ordering.end(), 0);

            this->optimum_.x = std::vector<int>(n_variables);

            // Generate random doubles in the range [0, 1] and convert them to integers (0 or 1).
            const auto random_values = common::random::pbo::uniform(n_variables, this->random_generator());
            for (int i = 0; i < n_variables; ++i)
                this->optimum_.x[i] = std::lround(random_values[i]); // Round to nearest integer

            this->optimum_.y = this->evaluate(this->optimum_.x);
            this->time_step = 0;
        }

        int step() override
        {
            // Using std::shuffle results in different behaviour on Windows versus on Ubuntu.
            portable_shuffle(comparison_ordering, this->random_generator);
            this->time_step += 1;
            return this->time_step;
        }

        /**
         * @brief Sorts the given bitstrings lexicographically based on the ranking permutation.
         *
         * @param bitstrings The list of bitstrings to be sorted.
         * @return A sorted list of bitstrings.
         */
        std::vector<std::vector<int>> rank(const std::vector<std::vector<int>> &bitstrings)
        {
            // Copy the bitstrings to avoid modifying the original list
            std::vector<std::vector<int>> sorted_bitstrings = bitstrings;

            // Return True iff a > b.
            auto comparator = [this](const std::vector<int> &a, const std::vector<int> &b) {
                for (const int index : this->comparison_ordering)
                {
                    if (a[index] != b[index])
                    {
                        if (this->optimum_.x[index] == 1)
                            return a[index] > b[index];
                        return a[index] < b[index];
                    }
                }
                return false;
            };

            std::sort(sorted_bitstrings.begin(), sorted_bitstrings.end(), comparator);

            // This is to update the evaluations count and best solution seen so far.
            for (const auto &bit_string : bitstrings)
                (*this)(bit_string);

            return sorted_bitstrings;
        }

        /**
         * @brief Returns the indices of the given bitstrings sorted lexicographically based on the ranking permutation.
         *
         * This function generates a list of indices representing the order in which the bitstrings would be sorted.
         * It uses the same comparison criteria as the rank function, but instead of sorting the bitstrings themselves,
         * it sorts their indices. This is useful for understanding the original positions of the bitstrings in the
         * sorted order.
         *
         * @param bitstrings The list of bitstrings for which the sorted indices are to be obtained.
         * @return A vector of indices indicating the sorted order of the bitstrings.
         */
        std::vector<int> rank_indices(const std::vector<std::vector<int>> &bitstrings)
        {
            // Create an index list from 0 to n-1
            std::vector<int> indices(bitstrings.size());
            std::iota(indices.begin(), indices.end(), 0);

            // Comparator using the comparison_ordering and optimum_.x from 'this' object
            auto comparator = [this, &bitstrings](const int i, const int j) {
                for (const int index : this->comparison_ordering)
                {
                    if (bitstrings[i][index] != bitstrings[j][index])
                    {
                        return this->optimum_.x[index] == 1 ? bitstrings[i][index] > bitstrings[j][index]
                                                            : bitstrings[i][index] < bitstrings[j][index];
                    }
                }
                return false;
            };

            // Sort the indices based on the comparator
            std::sort(indices.begin(), indices.end(), comparator);

            // This is to update the evaluations count and best solution seen so far.
            for (const auto &bit_string : bitstrings)
                (*this)(bit_string);

            return indices;
        }

        const std::vector<int> &get_comparison_ordering() const { return comparison_ordering; }

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
            double sum = 0;
            for (size_t i = 0; i < x.size(); ++i)
                sum += 1 - (x[i] ^ this->optimum_.x[i]);

            return sum;
        }

        //! overwrite do nothing
        double transform_objectives(const double y) override { return y; }

        //! overwrite do nothing
        std::vector<int> transform_variables(std::vector<int> x) override { return x; }
    };
} // namespace ioh::problem
