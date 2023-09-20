#pragma once

#include "cec_problem.hpp"

/// Namespace encapsulating the CEC benchmark problems, here particularly defining the Rastrigin problem class.
namespace ioh::problem::cec
{
    /// The Rastrigin class represents the Rastrigin optimization problem, inheriting necessary functionalities from the CECProblem template class.
    class Rastrigin final : public CECProblem<Rastrigin>
    {
    protected:

        /// Computes the Rastrigin objective function value at the given point.
        /// @param z A vector representing a point in the search space.
        /// @return The objective function value at the input point.
        double evaluate(const std::vector<double>& z) override
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

            ioh::problem::transformation::variables::scale_and_rotate(x, z, y, this->variables_shifts_[0], this->linear_transformations_[0], 5.12 / 100.0, 1, 1);

            return z;
        }

    public:

        /// Unique identifier for the Rastrigin problem in the CEC benchmark suite.
        inline static const int meta_problem_id = 1004;

        /// String name assigned to the Rastrigin problem in the CEC benchmark suite.
        inline static const std::string meta_name = "CEC_Rastrigin";

        /// Constructor initializing the Rastrigin problem with given instance number and variable count.
        /// @param instance Instance number for the problem.
        /// @param n_variables Number of variables for the problem.
        Rastrigin(const int instance, const int n_variables) :
            CECProblem(meta_problem_id, instance, n_variables, meta_name)
        {
            this->set_optimum();
        }
    };
}
