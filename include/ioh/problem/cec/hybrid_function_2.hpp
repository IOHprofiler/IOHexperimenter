#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    /// \brief CEC_HybridFunction2 class that inherits from CECProblem.
    class CEC_HybridFunction2 final : public CECProblem<CEC_HybridFunction2>
    {
    protected:
        /// \brief Evaluates the function with the transformed variables.
        /// \param prepared_y The transformed variables.
        /// \return The evaluation result.
        double evaluate(const std::vector<double>& prepared_y) override
        {
            double f = hf10(prepared_y);
            return f;
        }

        /// \brief Transforms the input variables.
        /// \param x The input variables.
        /// \return The transformed variables.
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            auto&& Os = this->variables_shifts_[0];
            auto&& Mr = this->linear_transformations_[0];
            auto&& S = this->input_permutation_;

            const int cf_num = 6;
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

            ioh::problem::transformation::variables::scale_and_rotate(x, z, y, Os, Mr, 1.0, 1, 1);

            for (int i = 0; i < nx; i++)
            {
                y[i] = z[S[i] - 1];
            }

            std::vector<double> hgbat_x(y.begin() + G[0], y.begin() + G[0] + G_nx[0]);
            std::vector<double> hgbat_z(hgbat_x.size());
            std::vector<double> hgbat_y(hgbat_x.size());
            ioh::problem::transformation::variables::scale_and_rotate(hgbat_x, hgbat_z, hgbat_y, Os, Mr, 5.0 / 100.0, 0, 0);

            std::vector<double> katsuura_x(y.begin() + G[1],
            y.begin() + G[1] + G_nx[1]);
            std::vector<double> katsuura_z(katsuura_x.size());
            std::vector<double> katsuura_y(katsuura_x.size());
            ioh::problem::transformation::variables::scale_and_rotate(katsuura_x, katsuura_z, katsuura_y, Os, Mr, 5.0 / 100.0, 0, 0);

            std::vector<double> ackley_x(y.begin() + G[2],
            y.begin() + G[2] + G_nx[2]);
            std::vector<double> ackley_z(ackley_x.size());
            std::vector<double> ackley_y(ackley_x.size());
            ioh::problem::transformation::variables::scale_and_rotate(ackley_x, ackley_z, ackley_y, Os, Mr, 1.0, 0, 0);

            std::vector<double> rastrigin_x(y.begin() + G[3], y.begin() + G[3] + G_nx[3]);
            std::vector<double> rastrigin_z(rastrigin_x.size());
            std::vector<double> rastrigin_y(rastrigin_x.size());
            ioh::problem::transformation::variables::scale_and_rotate(rastrigin_x, rastrigin_z, rastrigin_y, Os, Mr, 5.12 / 100.0, 0, 0);

            std::vector<double> schwefel_x(y.begin() + G[4],
            y.begin() + G[4] + G_nx[4]);
            std::vector<double> schwefel_z(schwefel_x.size());
            std::vector<double> schwefel_y(schwefel_x.size());
            ioh::problem::transformation::variables::scale_and_rotate(schwefel_x, schwefel_z, schwefel_y, Os, Mr, 1000.0 / 100.0, 0, 0);

            std::vector<double> schaffer_y(y.begin(), y.begin() + G_nx[5]);

            std::vector<double> prepared_y;

            // Start by reserving space for efficiency
            prepared_y.reserve(
                hgbat_z.size() \
                + katsuura_z.size() \
                + ackley_z.size() \
                + rastrigin_z.size() \
                + schwefel_z.size() \
                + schaffer_y.size() \
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
        /// \brief Meta problem ID.
        inline static const int meta_problem_id = 1007;
        /// \brief Meta name of the problem.
        inline static const std::string meta_name = "CEC_HybridFunction2";

        /// \brief Constructor of the CEC_HybridFunction2 class.
        /// \param instance The instance number of the problem.
        /// \param n_variables The number of variables.
        CEC_HybridFunction2(const int instance, const int n_variables) :
            CECProblem(meta_problem_id, instance, n_variables, meta_name)
        {
            this->set_optimum();
        }
    };
}
