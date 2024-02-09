#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    /// \brief CEC_HybridFunction2 class that inherits from CECProblem.
    class CEC_HybridFunction2 final : public CECProblem<CEC_HybridFunction2>
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
            constexpr int cf_num = 6;
            std::vector<double> fit(cf_num, 0.0);
            std::vector<int> g_nx(cf_num);
            std::vector<int> g(cf_num);
            const std::vector<double> gp = {0.1, 0.2, 0.2, 0.2, 0.1, 0.2};
            const int nx = static_cast<int>(x.size());

            int tmp = 0;
            for (int i = 0; i < cf_num - 1; i++)
            {
                g_nx[i] = static_cast<int>(std::ceil(gp[i] * nx));
                tmp += g_nx[i];
            }
            g_nx[cf_num - 1] = nx - tmp;

            g[0] = 0;
            for (int i = 1; i < cf_num; i++)
            {
                g[i] = g[i - 1] + g_nx[i - 1];
            }

            const std::vector<double> hgbat_z(x.begin() + g[0], x.begin() + g[0] + g_nx[0]);
            fit[0] = hgbat(hgbat_z);

            const std::vector<double> katsuura_z(x.begin() + g[1], x.begin() + g[1] + g_nx[1]);
            fit[1] = katsuura(katsuura_z);

            const std::vector<double> ackley_z(x.begin() + g[2], x.begin() + g[2] + g_nx[2]);
            fit[2] = ackley(ackley_z);

            const std::vector<double> rastrigin_z(x.begin() + g[3], x.begin() + g[3] + g_nx[3]);
            fit[3] = rastrigin(rastrigin_z);

            const std::vector<double> schwefel_z(x.begin() + g[4], x.begin() + g[4] + g_nx[4]);
            fit[4] = schwefel(schwefel_z);

            const std::vector<double> schaffer_y(x.begin() + g[5], x.begin() + g[5] + g_nx[5]);
            fit[5] = schaffer(schaffer_y);

            double &&f = std::accumulate(fit.begin(), fit.end(), 0.0);
            return f;
        }

        /// \brief Transforms the input variables.
        /// \param x The input variables.
        /// \return The transformed variables.
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            auto &&Os = this->variables_shifts_[0];
            auto &&Mr = this->linear_transformations_[0];
            auto &&S = this->input_permutation_;

            constexpr int cf_num = 6;
            std::vector<double> z(x.size());
            std::vector<double> y(x.size());
            std::vector<int> G_nx(cf_num);
            std::vector<int> G(cf_num);
            std::vector<double> Gp = {0.1, 0.2, 0.2, 0.2, 0.1, 0.2};
            int nx = x.size();

            int tmp = 0;
            for (int i = 0; i < cf_num - 1; i++)
            {
                G_nx[i] = static_cast<int>(std::ceil(Gp[i] * nx));
                tmp += G_nx[i];
            }
            G_nx[cf_num - 1] = nx - tmp;

            G[0] = 0;
            for (int i = 1; i < cf_num; i++)
            {
                G[i] = G[i - 1] + G_nx[i - 1];
            }

            transformation::variables::scale_and_rotate(x, z, y, Os, Mr, 1.0, true, true);

            for (int i = 0; i < nx; i++)
            {
                y[i] = z[S[i] - 1];
            }

            std::vector<double> hgbat_x(y.begin() + G[0], y.begin() + G[0] + G_nx[0]);
            std::vector<double> hgbat_z(hgbat_x.size());
            std::vector<double> hgbat_y(hgbat_x.size());
            transformation::variables::scale_and_rotate(hgbat_x, hgbat_z, hgbat_y, Os, Mr, 5.0 / 100.0, false, false);

            std::vector<double> katsuura_x(y.begin() + G[1],
                                           y.begin() + G[1] + G_nx[1]);
            std::vector<double> katsuura_z(katsuura_x.size());
            std::vector<double> katsuura_y(katsuura_x.size());
            transformation::variables::scale_and_rotate(katsuura_x, katsuura_z, katsuura_y, Os, Mr, 5.0 / 100.0, false,
                                                        false);

            std::vector<double> ackley_x(y.begin() + G[2],
                                         y.begin() + G[2] + G_nx[2]);
            std::vector<double> ackley_z(ackley_x.size());
            std::vector<double> ackley_y(ackley_x.size());
            transformation::variables::scale_and_rotate(ackley_x, ackley_z, ackley_y, Os, Mr, 1.0, false, false);

            std::vector<double> rastrigin_x(y.begin() + G[3], y.begin() + G[3] + G_nx[3]);
            std::vector<double> rastrigin_z(rastrigin_x.size());
            std::vector<double> rastrigin_y(rastrigin_x.size());
            transformation::variables::scale_and_rotate(rastrigin_x, rastrigin_z, rastrigin_y, Os, Mr, 5.12 / 100.0,
                                                        false, false);

            std::vector<double> schwefel_x(y.begin() + G[4],
                                           y.begin() + G[4] + G_nx[4]);
            std::vector<double> schwefel_z(schwefel_x.size());
            std::vector<double> schwefel_y(schwefel_x.size());
            transformation::variables::scale_and_rotate(schwefel_x, schwefel_z, schwefel_y, Os, Mr, 1000.0 / 100.0,
                                                        false, false);

            std::vector<double> schaffer_y(y.begin(), y.begin() + G_nx[5]);

            std::vector<double> prepared_y;

            // Start by reserving space for efficiency
            prepared_y.reserve(
                hgbat_z.size()
                + katsuura_z.size()
                + ackley_z.size()
                + rastrigin_z.size()
                + schwefel_z.size()
                + schaffer_y.size()
                );

            prepared_y.insert(prepared_y.end(), hgbat_z.begin(), hgbat_z.end());
            prepared_y.insert(prepared_y.end(), katsuura_z.begin(), katsuura_z.end());
            prepared_y.insert(prepared_y.end(), ackley_z.begin(), ackley_z.end());
            prepared_y.insert(prepared_y.end(), rastrigin_z.begin(), rastrigin_z.end());
            prepared_y.insert(prepared_y.end(), schwefel_z.begin(), schwefel_z.end());
            prepared_y.insert(prepared_y.end(), schaffer_y.begin(), schaffer_y.end());

            return prepared_y;
        }

    public:
        /// \brief Constructor of the CEC_HybridFunction2 class.
        /// \param instance The instance number of the problem.
        /// \param n_variables The number of variables.
        CEC_HybridFunction2(const int instance, const int n_variables) :
            CECProblem(1007, instance, n_variables, "CEC_HybridFunction2")
        {
        }
    };
}
