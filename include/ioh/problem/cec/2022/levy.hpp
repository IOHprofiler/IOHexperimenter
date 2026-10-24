#pragma once

#include "cec_problem.hpp"

/// Namespace encapsulating the CEC benchmark problems, here particularly defining the Levy problem class.
namespace ioh::problem::cec2022
{
    /// The Levy problem of the CEC2022 suite. Derives from CEC2022; its registry id and name are
    /// registry_id and registry_name, and the compiled library registers it (src/registry).
    class Levy final : public CEC2022
    {
    public:
        //! The id in the registry, also used by the constructor
        static constexpr int registry_id = 1005;
        //! The name in the registry, also used by the constructor
        static constexpr const char *registry_name = "CEC2022Levy";

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
        Levy(const int instance, const int n_variables) : CEC2022(registry_id, instance, n_variables, registry_name)
        {
        }
    };
}
