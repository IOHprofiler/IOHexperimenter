#pragma once

#include "cec_problem.hpp"

/// Namespace encapsulating the CEC benchmark problems, here particularly defining the Rastrigin problem class.
namespace ioh::problem::cec
{
    /// The Rastrigin class represents the Rastrigin optimization problem, inheriting necessary functionalities from the CECProblem template class.
    class CEC_Rastrigin final : public CECProblem<CEC_Rastrigin>
    {
    protected:
        /// Computes the Rastrigin objective function value at the given point.
        /// @param z A vector representing a point in the search space.
        /// @return The objective function value at the input point.
        double evaluate(const std::vector<double> &z) override
        {
            double f = rastrigin(z);
            return f;
        }

        /// Performs a defined transformation on the input variables to make them suitable for the Rastrigin problem evaluation.
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
        /// Constructor initializing the Rastrigin problem with given instance number and variable count.
        /// @param instance Instance number for the problem.
        /// @param n_variables Number of variables for the problem.
        CEC_Rastrigin(const int instance, const int n_variables) :
            CECProblem(1004, instance, n_variables, "CEC_Rastrigin")
        {
        }
    };
}
