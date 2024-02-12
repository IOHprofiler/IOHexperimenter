#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec2022
{
    /// \brief Class representing the second composition function in the CEC benchmark suite.
    class CompositionFunction2 final : public CEC2022Problem<CompositionFunction2>
    {
    protected:
        /**
         * @brief Evaluates the objective function for the given input vector.
         *
         * @param x The input vector.
         * @return The objective function value.
         */
        double evaluate(const std::vector<double> &x) override
        {
            const std::vector<double> deltas = {20, 10, 10};
            const std::vector<double> biases = {0, 200, 100};
            const std::vector<double> sh_rates = {1000.0 / 100.0, 5.12 / 100.0, 5.0 / 100.0};
            const std::vector<bool> s_flags = {true, true, true};
            const std::vector<bool> r_flags = {false, true, true};
            const std::vector<double> f_scale = {1e+4, 1e+4, 1e+4};
            const std::vector<RealFunction> functions = {schwefel, rastrigin, hgbat};
          
            return calculate_composite(x, this->variables_shifts_, this->linear_transformations_, deltas, biases,
                                       sh_rates, s_flags, r_flags, f_scale, functions);
        }


    public:
        /**
         * @brief Constructs a CEC_CompositionFunction2 instance.
         *
         * @param instance The instance number of the problem.
         * @param n_variables The number of variables in the problem.
         */
        CompositionFunction2(const int instance, const int n_variables) :
            CEC2022Problem(1010, instance, n_variables, "CEC2022CompositionFunction2")
        {
        }
    };
}
