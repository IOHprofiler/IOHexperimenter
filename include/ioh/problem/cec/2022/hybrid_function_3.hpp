#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    /// \brief CEC_HybridFunction3 class that inherits from CECProblem.
    class CEC_HybridFunction3 final : public CECProblem<CEC_HybridFunction3>
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
            constexpr int cf_num = 5;
            std::vector<double> fit(cf_num, 0);
            std::vector<int> g_nx(cf_num);
            std::vector<int> g(cf_num);
            const std::vector<double> gp = {0.3, 0.2, 0.2, 0.1, 0.2};
            const int nx = static_cast<int>(x.size());

            int tmp = 0;
            for (int i = 0; i < cf_num - 1; ++i)
            {
                g_nx[i] = static_cast<int>(std::ceil(gp[i] * nx));
                tmp += g_nx[i];
            }

            g_nx[cf_num - 1] = nx - tmp;
            if (g_nx[cf_num - 1] < 0)
            {
                g_nx[cf_num - 1] = 0;
            }

            g[0] = 0;
            for (int i = 1; i < cf_num; ++i)
            {
                g[i] = g[i - 1] + g_nx[i - 1];
            }

            const std::vector<double> katsuura_z(x.begin() + g[0], x.begin() + g[0] + g_nx[0]);
            fit[0] = katsuura(katsuura_z);

            const std::vector<double> happycat_z(x.begin() + g[1], x.begin() + g[1] + g_nx[1]);
            fit[1] = happycat(happycat_z);

            const std::vector<double> griewank_rosenbrock_z(x.begin() + g[2],
                                                            x.begin() + g[2] + g_nx[2]);
            fit[2] = griewank_rosenbrock(griewank_rosenbrock_z);

            const std::vector<double> schwefel_z(x.begin() + g[3], x.begin() + g[3] + g_nx[3]);
            fit[3] = schwefel(schwefel_z);

            const std::vector<double> ackley_z(x.begin() + g[4], x.begin() + g[4] + g_nx[4]);
            fit[4] = ackley(ackley_z);

            return std::accumulate(fit.begin(), fit.end(), 0.0);
        }

        /// \brief Transforms the input variables.
        /// \param x The input variables.
        /// \return The transformed variables.
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            auto &&Os = this->variables_shifts_[0];
            auto &&Mr = this->linear_transformations_[0];
            auto &&S = this->input_permutation_;

            constexpr int cf_num = 5;
            std::vector<int> G_nx(cf_num);
            std::vector<int> G(cf_num);
            std::vector<double> Gp = {0.3, 0.2, 0.2, 0.1, 0.2};

            int tmp = 0;
            for (int i = 0; i < cf_num - 1; ++i)
            {
                G_nx[i] = std::ceil(Gp[i] * x.size());
                tmp += G_nx[i];
            }
            G_nx[cf_num - 1] = x.size() - tmp;
            if (G_nx[cf_num - 1] < 0)
            {
                G_nx[cf_num - 1] = 0;
            }

            G[0] = 0;
            for (int i = 1; i < cf_num; ++i)
            {
                G[i] = G[i - 1] + G_nx[i - 1];
            }

            std::vector<double> z(x.size());
            std::vector<double> y(x.size());
            transformation::variables::scale_and_rotate(x, z, y, Os, Mr, 1.0, true, true);

            for (size_t i = 0; i < x.size(); ++i)
            {
                y[i] = z[S[i] - 1];
            }

            std::vector<double> katsuura_x(y.begin() + G[0], y.begin() + G[0] + G_nx[0]);
            std::vector<double> katsuura_z(katsuura_x.size());
            std::vector<double> katsuura_y(katsuura_x.size());
            transformation::variables::scale_and_rotate(katsuura_x, katsuura_z, katsuura_y, Os, Mr, 5.0 / 100.0, false,
                                                        false);

            std::vector<double> happycat_x(y.begin() + G[1], y.begin() + G[1] + G_nx[1]);
            std::vector<double> happycat_z(happycat_x.size());
            std::vector<double> happycat_y(happycat_x.size());
            transformation::variables::scale_and_rotate(happycat_x, happycat_z, happycat_y, Os, Mr, 5.0 / 100.0, false,
                                                        false);

            std::vector<double> grie_rosen_x(y.begin() + G[2], y.begin() + G[2] + G_nx[2]);
            std::vector<double> grie_rosen_z(grie_rosen_x.size());
            std::vector<double> grie_rosen_y(grie_rosen_x.size());
            transformation::variables::scale_and_rotate(grie_rosen_x, grie_rosen_z, grie_rosen_y, Os, Mr, 5.0 / 100.0,
                                                        false, false);

            std::vector<double> schwefel_x(y.begin() + G[3], y.begin() + G[3] + G_nx[3]);
            std::vector<double> schwefel_z(schwefel_x.size());
            std::vector<double> schwefel_y(schwefel_x.size());
            transformation::variables::scale_and_rotate(schwefel_x, schwefel_z, schwefel_y, Os, Mr, 1000.0 / 100.0,
                                                        false, false);

            std::vector<double> ackley_x(y.begin() + G[4], y.begin() + G[4] + G_nx[4]);
            std::vector<double> ackley_z(ackley_x.size());
            std::vector<double> ackley_y(ackley_x.size());
            transformation::variables::scale_and_rotate(ackley_x, ackley_z, ackley_y, Os, Mr, 1.0, false, false);

            std::vector<double> prepared_y;

            prepared_y.reserve(
                katsuura_z.size()
                + happycat_z.size()
                + grie_rosen_z.size()
                + schwefel_z.size()
                + ackley_z.size()
                );

            prepared_y.insert(prepared_y.end(), katsuura_z.begin(), katsuura_z.end());
            prepared_y.insert(prepared_y.end(), happycat_z.begin(), happycat_z.end());
            prepared_y.insert(prepared_y.end(), grie_rosen_z.begin(), grie_rosen_z.end());
            prepared_y.insert(prepared_y.end(), schwefel_z.begin(), schwefel_z.end());
            prepared_y.insert(prepared_y.end(), ackley_z.begin(), ackley_z.end());

            return prepared_y;
        }

    public:
        /// \brief Constructor of the CEC_HybridFunction3 class.
        /// \param instance The instance number of the problem.
        /// \param n_variables The number of variables.
        CEC_HybridFunction3(const int instance, const int n_variables) :
            CECProblem(1008, instance, n_variables, "CEC_HybridFunction3")
        {
        }
    };
}
