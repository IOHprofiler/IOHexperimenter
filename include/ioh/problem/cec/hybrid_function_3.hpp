#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    class HybridFunction3 final : public CECProblem<HybridFunction3>
    {
    protected:

        double evaluate(const std::vector<double>& prepared_y) override
        {
            double f = hf06(prepared_y);
            return f;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            auto&& Os = this->variables_shifts_[0];
            auto&& Mr = this->linear_transformations_[0];
            auto&& S = this->input_permutation_;

            const int cf_num = 5;
            std::vector<double> fit(cf_num, 0);
            std::vector<int> G_nx(cf_num);
            std::vector<int> G(cf_num);
            std::vector<double> Gp = {0.3, 0.2, 0.2, 0.1, 0.2};

            int tmp = 0;
            for (int i = 0; i < cf_num - 1; ++i) {
                G_nx[i] = std::ceil(Gp[i] * x.size());
                tmp += G_nx[i];
            }
            G_nx[cf_num - 1] = x.size() - tmp;
            if (G_nx[cf_num - 1] < 0) {
                G_nx[cf_num - 1] = 0;
            }

            G[0] = 0;
            for (int i = 1; i < cf_num; ++i) {
                G[i] = G[i - 1] + G_nx[i - 1];
            }

            std::vector<double> z(x.size());
            std::vector<double> y(x.size());
            scale_and_rotate(x, z, y, Os, Mr, 1.0, 1, 1);

            for (size_t i = 0; i < x.size(); ++i) {
                y[i] = z[S[i] - 1];
            }

            std::vector<double> katsuura_func_x(y.begin() + G[0], y.begin() + G[0] + G_nx[0]);
            std::vector<double> katsuura_func_z(katsuura_func_x.size());
            std::vector<double> katsuura_func_y(katsuura_func_x.size());
            scale_and_rotate(katsuura_func_x, katsuura_func_z, katsuura_func_y, Os, Mr, 5.0 / 100.0, 0, 0);

            std::vector<double> happycat_func_x(y.begin() + G[1], y.begin() + G[1] + G_nx[1]);
            std::vector<double> happycat_func_z(happycat_func_x.size());
            std::vector<double> happycat_func_y(happycat_func_x.size());
            scale_and_rotate(happycat_func_x, happycat_func_z, happycat_func_y, Os, Mr, 5.0 / 100.0, 0, 0);

            std::vector<double> grie_rosen_func_x(y.begin() + G[2], y.begin() + G[2] + G_nx[2]);
            std::vector<double> grie_rosen_func_z(grie_rosen_func_x.size());
            std::vector<double> grie_rosen_func_y(grie_rosen_func_x.size());
            scale_and_rotate(grie_rosen_func_x, grie_rosen_func_z, grie_rosen_func_y, Os, Mr, 5.0 / 100.0, 0, 0);

            std::vector<double> schwefel_func_x(y.begin() + G[3], y.begin() + G[3] + G_nx[3]);
            std::vector<double> schwefel_func_z(schwefel_func_x.size());
            std::vector<double> schwefel_func_y(schwefel_func_x.size());
            scale_and_rotate(schwefel_func_x, schwefel_func_z, schwefel_func_y, Os, Mr, 1000.0 / 100.0, 0, 0);

            std::vector<double> ackley_func_x(y.begin() + G[4], y.begin() + G[4] + G_nx[4]);
            std::vector<double> ackley_func_z(ackley_func_x.size());
            std::vector<double> ackley_func_y(ackley_func_x.size());
            scale_and_rotate(ackley_func_x, ackley_func_z, ackley_func_y, Os, Mr, 1.0, 0, 0);

            std::vector<double> prepared_y;

            prepared_y.reserve(
                katsuura_func_z.size() \
                + happycat_func_z.size() \
                + grie_rosen_func_z.size() \
                + schwefel_func_z.size() \
                + ackley_func_z.size() \
            );

            prepared_y.insert(prepared_y.end(), katsuura_func_z.begin(), katsuura_func_z.end());
            prepared_y.insert(prepared_y.end(), happycat_func_z.begin(), happycat_func_z.end());
            prepared_y.insert(prepared_y.end(), grie_rosen_func_z.begin(), grie_rosen_func_z.end());
            prepared_y.insert(prepared_y.end(), schwefel_func_z.begin(), schwefel_func_z.end());
            prepared_y.insert(prepared_y.end(), ackley_func_z.begin(), ackley_func_z.end());

            return prepared_y;
        }

    public:

        inline static const int meta_problem_id = 8;
        inline static const std::string meta_name = "CEC_HybridFunction3";

        HybridFunction3(const int instance, const int n_variables) :
            CECProblem(meta_problem_id, instance, n_variables, meta_name)
        {
            this->set_optimum();
        }
    };
}
