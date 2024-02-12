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

            for (size_t i = 0; i < x.size(); ++i)
            {
                y[i] = z[this->input_permutation_[i] - 1];
            }
            return y;

            // auto &&Os = this->variables_shifts_[0];
            // auto &&Mr = this->linear_transformations_[0];
            // auto &&S = this->input_permutation_;
            //
            // constexpr int cf_num = 6;
            // std::vector<double> z(x.size());
            // std::vector<double> y(x.size());
            // std::vector<int> G_nx(cf_num);
            // std::vector<int> G(cf_num);
            // std::vector<double> Gp = {0.1, 0.2, 0.2, 0.2, 0.1, 0.2};
            // int nx = x.size();
            //
            // int tmp = 0;
            // for (int i = 0; i < cf_num - 1; i++)
            // {
            //     G_nx[i] = static_cast<int>(std::ceil(Gp[i] * nx));
            //     tmp += G_nx[i];
            // }
            // G_nx[cf_num - 1] = nx - tmp;
            //
            // G[0] = 0;
            // for (int i = 1; i < cf_num; i++)
            // {
            //     G[i] = G[i - 1] + G_nx[i - 1];
            // }
            //
            // transformation::variables::scale_and_rotate(x, z, y, Os, Mr, 1.0, true, true);
            //
            // for (int i = 0; i < nx; i++)
            // {
            //     y[i] = z[S[i] - 1];
            // }
            // std::vector<double> hgbat_x(y.begin() + G[0], y.begin() + G[0] + G_nx[0]);
            // std::vector<double> katsuura_x(y.begin() + G[1],
            //                                y.begin() + G[1] + G_nx[1]);
            //
            //
            // std::vector<double> ackley_x(y.begin() + G[2],
            //                              y.begin() + G[2] + G_nx[2]);
            //
            // std::vector<double> rastrigin_x(y.begin() + G[3], y.begin() + G[3] + G_nx[3]);            
            //
            //
            // std::vector<double> schwefel_x(y.begin() + G[4],
            //                                y.begin() + G[4] + G_nx[4]);
            //
            // std::vector<double> schaffer_y(y.begin(), y.begin() + G_nx[5]);
            //
            // std::vector<double> prepared_y;
            //
            // // Start by reserving space for efficiency
            // prepared_y.reserve(x.size());
            //
            // prepared_y.insert(prepared_y.end(), hgbat_x.begin(), hgbat_x.end());
            // prepared_y.insert(prepared_y.end(), katsuura_x.begin(), katsuura_x.end());
            // prepared_y.insert(prepared_y.end(), ackley_x.begin(), ackley_x.end());
            // prepared_y.insert(prepared_y.end(), rastrigin_x.begin(), rastrigin_x.end());
            // prepared_y.insert(prepared_y.end(), schwefel_x.begin(), schwefel_x.end());
            // prepared_y.insert(prepared_y.end(), schaffer_y.begin(), schaffer_y.end());

            // common::print(y);
            // common::print(prepared_y);
            // std::cout << G_nx[5] << std::endl;
            //
            // return prepared_y;
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
