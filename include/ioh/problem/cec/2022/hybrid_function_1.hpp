#pragma once

#include "cec_problem.hpp"

//! Namespace encapsulating all elements of the IOHexperimenter project
namespace ioh::problem::cec2022
{
    //! A class representing the HybridFunction1 problem, derived from the CECProblem class.
    /*!
     * This class represents a specific optimization problem which is a hybrid of several other functions.
     * It overrides the evaluate and transform_variables methods to provide the specific implementation for this
     * problem.
     */
    class HybridFunction1 final : public CEC2022Problem<HybridFunction1>
    {
    protected:
        /**
         * @brief Computes the hf02 function value for the input vector.
         *
         * The hf02 function is a hybrid function that combines several benchmark functions to create a complex,
         * multimodal landscape. It is used to evaluate the performance of optimization algorithms in navigating
         * complex, high-dimensional spaces.
         *
         * @param x Input vector containing the coordinates in the domain space.
         * @return The function value at the point specified by the input vector.
         */
        double evaluate(const std::vector<double> &x) override
        {
            return calculate_hybrid(x, {0.4, 0.4, 0.2}, {1, 5.0 / 100.0, 5.12 / 100.0}, {bent_cigar, hgbat, rastrigin});
        }

        //! Transforms the input variables according to the problem's specific transformations.
        /*!
         * This method applies several transformations to the input variables, including scaling, rotation, and
         * permutation. It then divides the transformed variables into several groups and applies additional
         * transformations to each group.
         *
         * \param x A vector of input variables.
         * \return A vector of transformed variables, ready to be evaluated by the objective function.
         */
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
        //! Constructor for the HybridFunction1 class.
        /*!
         * Initializes a new instance of the HybridFunction1 class, setting up the problem with the specified
         * instance and number of variables.
         *
         * \param instance The instance number of the problem.
         * \param n_variables The number of variables for the problem.
         */
        HybridFunction1(const int instance, const int n_variables) :
            CEC2022Problem(1006, instance, n_variables, "CEC2022HybridFunction1")
        {
        }
    };
} // namespace ioh::problem::cec2022
