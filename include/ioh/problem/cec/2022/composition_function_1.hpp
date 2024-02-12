#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec2022
{
    /// \brief Class representing the first composition function in the CEC benchmark suite.
    class CompositionFunction1 final : public CEC2022Problem<CompositionFunction1>
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
            const std::vector<double> deltas = {10, 20, 30, 40, 50};
            const std::vector<double> biases = {0, 200, 300, 100, 400};
            const std::vector<double> sh_rates = {2.048 / 100.0, 1, 1, 1, 1};
            const std::vector<bool> s_flags = {true, true, true, true, true};
            const std::vector<bool> r_flags = {true, false, true, true, false};
            const std::vector<double> f_scale = {1e+4, 1e+10, 1e+30, 1e+10, 1e+10};
            const std::vector<RealFunction> functions = {rosenbrock, ellipse, bent_cigar, discus, ellipse};

            return calculate_composite(x, this->variables_shifts_, this->linear_transformations_, deltas, biases,
                                       sh_rates, s_flags, r_flags, f_scale, functions);
                
        }

    public:
        /**
         * @brief Constructs a CEC_CompositionFunction1 instance.
         *
         * @param instance The instance number of the problem.
         * @param n_variables The number of variables in the problem.
         */
        CompositionFunction1(const int instance, const int n_variables) :
            CEC2022Problem(1009, instance, n_variables, "CEC2022CompositionFunction1")
        {
        }
    };
}
