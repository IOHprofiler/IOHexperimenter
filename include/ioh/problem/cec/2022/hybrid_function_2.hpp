#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec2022
{
    /// \brief HybridFunction2 class that inherits from CECProblem.
    class HybridFunction2 final : public CEC2022Problem<HybridFunction2>
    {
    protected:
        /**
         * @brief Computes the hf06 function value for the input vector.
         *
         * The hf06 function is another hybrid function that integrates several benchmark functions to create a
         * challenging optimization landscape characterized by multiple local minima and a global minimum. It is
         * utilized to test the robustness and efficiency of optimization algorithms.
         *
         * @param prepared_y Input vector containing the coordinates in the domain space.
         * @return The function value at the point specified by the input vector.
         */
        double evaluate(const std::vector<double> &x) override
        {
            return calculate_hybrid(x, {0.1, 0.2, 0.2, 0.2, 0.1, 0.2}, {0.05, 0.05, 1, 5.12 / 100.0, 10, 1}, 
                                    {hgbat, katsuura, ackley, rastrigin, schwefel, schaffer}); 
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

            for (size_t i = 0; i < x.size(); i++)
            {
                y[i] = z[this->input_permutation_[i] - 1];
            }
             
            const int n = static_cast<int>(x.size());
            int n_shaffer = n;
            for (const auto gp : {0.1, 0.2, 0.2, 0.2, 0.1})
            {
                n_shaffer -= static_cast<int>(std::ceil(gp * n));
            }

            for (int i = 0; i < n_shaffer; i++)
            {
                y[y.size() - n_shaffer + i] = y[i];
            }

            return y;
            
        }

    public:
        /// \brief Constructor of the HybridFunction2 class.
        /// \param instance The instance number of the problem.
        /// \param n_variables The number of variables.
        HybridFunction2(const int instance, const int n_variables) :
            CEC2022Problem(1007, instance, n_variables, "CEC2022HybridFunction2")
        {
        }
    };
}
