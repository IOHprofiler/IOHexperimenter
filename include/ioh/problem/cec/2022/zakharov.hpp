#pragma once

#include "cec_problem.hpp"

/// Namespace enclosing the Implementation of Zakharov problem which is a part of CEC benchmark problems.
namespace ioh::problem::cec2022
{
    /// The Zakharov problem of the CEC2022 suite. Derives from CEC2022; its registry id and name are
    /// registry_id and registry_name, and the compiled library registers it (src/registry).
    /// This class encapsulates the Zakharov problem for the CEC benchmark suite.
    class Zakharov final : public CEC2022
    {
    public:
        //! The id in the registry, also used by the constructor
        static constexpr int registry_id = 1001;
        //! The name in the registry, also used by the constructor
        static constexpr const char *registry_name = "CEC2022Zakharov";

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
        Zakharov(const int instance, const int n_variables) : CEC2022(registry_id, instance, n_variables, registry_name)
        {
        }
    };
}
