#pragma once

#include "cec_problem.hpp"

/// Namespace encapsulating the CEC benchmark problems, here particularly defining the Expanded Schaffer F7 problem class.
namespace ioh::problem::cec2022
{
    /// The Expanded Schaffer F7 problem of the CEC2022 suite. Derives from CEC2022; its registry id and name are
    /// registry_id and registry_name, and the compiled library registers it (src/registry).
    class SchafferF7 final : public CEC2022
    {
    public:
        //! The id in the registry, also used by the constructor
        static constexpr int registry_id = 1003;
        //! The name in the registry, also used by the constructor
        static constexpr const char *registry_name = "CEC2022SchafferF7";

    protected:
        /// Computes the Schaffer F7 objective function value at the given point.
        /// @param y A vector representing a point in the search space.
        /// @return The objective function value at the input point.
        double evaluate(const std::vector<double> &y) override
        {
            return schaffer(y);
        }

        /// Performs a defined transformation on the input variables to make them suitable for the Expanded Schaffer F7 problem evaluation.
        /// @param x A vector of input variables.
        /// @return A vector containing the transformed variables.
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            std::vector<double> y(x.size()), z(x.size());

            transformation::variables::scale_and_rotate(x, z, y, this->variables_shifts_[0],
                                                        this->linear_transformations_[0], 0.5 / 100.0, true, true);

            return y;
        }

    public:
        /// Constructor initializing the Schaffer F7 problem with given instance number and variable count.
        /// @param instance Instance number for the problem.
        /// @param n_variables Number of variables for the problem.
        SchafferF7(const int instance, const int n_variables) :
            CEC2022(registry_id, instance, n_variables, registry_name)
        {
        }
    };
}
