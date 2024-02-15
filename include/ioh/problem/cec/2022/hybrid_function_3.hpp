#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec2022
{
    /// \brief HybridFunction3 class that inherits from CECProblem.
    class HybridFunction3 final : public CEC2022Problem<HybridFunction3>
    {
    protected:
        /**
         * @brief Computes the hf06 function value for the input vector.
         *
         * The hf06 function is another hybrid function that integrates several benchmark functions to create a
         * challenging optimization landscape characterized by multiple local minima and a global minimum. It is
         * utilized to test the robustness and efficiency of optimization algorithms.
         *
         * @param x Input vector containing the coordinates in the domain space.
         * @return The function value at the point specified by the input vector.
         */
        double evaluate(const std::vector<double> &x) override
        {
            return calculate_hybrid(x, {0.3, 0.2, 0.2, 0.1, 0.2},
                                    {
                                        5.0 / 100.0,
                                        5.0 / 100.0,
                                        5.0 / 100.0,
                                        10, 1,
                                    }, 
                                    {katsuura, happycat, griewank_rosenbrock, schwefel, ackley}); 
        }

        /// \brief Transforms the input variables.
        /// \param x The input variables.
        /// \return The transformed variables.
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            std::vector<double> z(x.size());
            std::vector<double> y(x.size());

            transformation::variables::scale_and_rotate(x, z, y, this->variables_shifts_[0],
                                                        this->linear_transformations_[0], 1.0, true, true);

            for (size_t i = 0; i < x.size(); ++i)
            {
                y[i] = z[this->input_permutation_[i] - 1];
            }
            return y;
        }

    public:
        /// \brief Constructor of the HybridFunction3 class.
        /// \param instance The instance number of the problem.
        /// \param n_variables The number of variables.
        HybridFunction3(const int instance, const int n_variables) :
            CEC2022Problem(1008, instance, n_variables, "CEC2022HybridFunction3")
        {
        }
    };
}
