#pragma once
/**
 * @file dynamic_bin_val_powers_of_two.hpp
 * @brief Contains the declaration of the DynamicBinValPowersOfTwo class, which represents dynamic binary value
 *        problems in the context of IOH.
 */

#include "dynamic_bin_val.hpp"

namespace ioh::problem::dynamic_bin_val
{
    /**
     * @class PowersOfTwo
     * @brief This class serves to represent dynamic binary value problems within the context of Iterative
     *        Optimization Heuristics (IOH).
     *
     *        PowersOfTwo: takes a value among the powers 2**1, 2**2, 2**3, ..., 2**31 / n_variables for
     * each weight at each time_step
     *
     *        Inheriting functionalities from the IntegerSingleObjective, it also integrates functionalities
     *        for automatic registration of the problem type into various data structures. This facilitates
     *        easier management and retrieval of problem instances, while encapsulating characteristics and
     *        behaviours specific to dynamic binary value problems. It holds vital data members such as
     *        time_step and weights, which are crucial in depicting the dynamic aspects and unique features
     *        of these problem instances.
     */
    class PowersOfTwo final : public DynamicBinValProblem<PowersOfTwo>
    {
    protected:
        void update_weights() override
        {
            const int subtract_bits = static_cast<int>(log2(static_cast<double>(this->weights.size())));
            // Use the `uniform` function to generate values in a range suitable for exponents
            const auto random_exponents = common::random::pbo::uniform(
                this->weights.size(), this->random_generator(), 1, 31 - subtract_bits - 1);

            // Reinitialize the weights with powers of 2 using the generated exponent values
            for (size_t i = 0; i < this->weights.size(); ++i)
                this->weights[i] = 1 << static_cast<int>(std::round(random_exponents[i]));
        }

    public:
        /**
         * @brief Constructs a new instance of DynamicBinValPowersOfTwo.
         *
         * @param instance the random instance of the problem
         * @param n_variables The dimension of the problem, representing the size of the search space and
         *                    indicating the number of variables in the problem.
         */
        PowersOfTwo(const int instance, const int n_variables) :
            DynamicBinValProblem(10'002, instance, n_variables, "DynamicBinValPowersOfTwo")
        {
            step();
        }
    };
} // namespace ioh::problem
