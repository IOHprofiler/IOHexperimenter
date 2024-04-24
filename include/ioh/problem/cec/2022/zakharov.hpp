#pragma once

#include "cec_problem.hpp"

/// Namespace enclosing the Implementation of Zakharov problem which is a part of CEC benchmark problems.
namespace ioh::problem::cec2022
{
    /// Zakharov class inherits from the template CECProblem with Zakharov as the template parameter.
    /// This class encapsulates the Zakharov problem for the CEC benchmark suite.
    class Zakharov final : public CEC2022Problem<Zakharov>
    {
    protected:
        /// Evaluates the objective function for the Zakharov problem.
        /// @param x A vector representing a point in the search space.
        /// @return The objective function value at point x.
        double evaluate(const std::vector<double> &x) override
        {
            return zakharov(x);
        }

        /// Transforms the variables using scale and rotate operations.
        /// It modifies the input variables to suit the problem's specific transformation requirements.
        /// @param x A vector of input variables.
        /// @return A transformed vector of variables which are suitable for the Zakharov problem.
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            std::vector<double> y(x.size()), z(x.size());

            transformation::variables::scale_and_rotate(x, z, y, this->variables_shifts_[0],
                                                        this->linear_transformations_[0], 1.0, true, true);

            return z;
        }

    public:
        /// Constructs a Zakharov problem instance.
        /// @param instance An integer representing the problem instance number.
        /// @param n_variables An integer representing the number of variables for the problem.
        Zakharov(const int instance, const int n_variables) : CEC2022Problem(1001, instance, n_variables, "CEC2022Zakharov")
        {
        }
    };
}
