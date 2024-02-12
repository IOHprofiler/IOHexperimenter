#pragma once

#include "cec_problem.hpp"

/// Namespace encapsulating the CEC benchmark problems, here particularly defining the Levy problem class.
namespace ioh::problem::cec2022
{
    /// The Levy class represents the Levy optimization problem, inheriting necessary functionalities from the CECProblem template class.
    class Levy final : public CEC2022Problem<Levy>
    {
    protected:
        /// Computes the Levy objective function value at the given point.
        /// @param z A vector representing a point in the search space.
        /// @return The objective function value at the input point.
        double evaluate(const std::vector<double> &z) override
        {
            return levy(z);
        }

        /// Performs a defined transformation on the input variables to make them suitable for the Levy problem evaluation.
        /// @param x A vector of input variables.
        /// @return A vector containing the transformed variables.
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            std::vector<double> y(x.size()), z(x.size());

            transformation::variables::scale_and_rotate(x, z, y, this->variables_shifts_[0],
                                                        this->linear_transformations_[0], 5.12 / 100.0, true, true);

            return z;
        }

    public:
        /// Constructor initializing the Levy problem with given instance number and variable count.
        /// @param instance Instance number for the problem.
        /// @param n_variables Number of variables for the problem.
        Levy(const int instance, const int n_variables) : CEC2022Problem(1005, instance, n_variables, "CEC2022Levy")
        {
        }
    };
}
