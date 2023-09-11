#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    class HybridFunction1 final : public CECProblem<HybridFunction1>
    {
    protected:

        double evaluate(const std::vector<double>& prepared_y) override
        {
            double f = hf02(prepared_y);
            return f;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            auto&& Os = this->variables_shifts_[0];
            auto&& Mr = this->linear_transformations_[0];
            auto&& S = this->input_permutation_;

            const int cf_num = 3;
            int nx = x.size();
            std::vector<double> z(nx);
            std::vector<double> y(nx);
            std::vector<double> fit(cf_num);
            std::vector<int> G(cf_num);
            std::vector<int> G_nx(cf_num);
            std::vector<double> Gp = {0.4, 0.4, 0.2};

            int tmp = 0;
            for (int i = 0; i < cf_num - 1; ++i) {
                G_nx[i] = std::ceil(Gp[i] * nx);
                tmp += G_nx[i];
            }
            G_nx[cf_num - 1] = nx - tmp;

            G[0] = 0;
            for (int i = 1; i < cf_num; ++i) {
                G[i] = G[i - 1] + G_nx[i - 1];
            }

            ioh::problem::transformation::scale_and_rotate(x, z, y, Os, Mr, 1.0, 1, 1);

            for (int i = 0; i < nx; ++i) {
                y[i] = z[S[i] - 1];
            }

            std::vector<double> bent_cigar_x(y.begin() + G[0], y.begin() + G[0] + G_nx[0]);
            std::vector<double> bent_cigar_z(bent_cigar_x.size());
            std::vector<double> bent_cigar_y(bent_cigar_x.size());
            ioh::problem::transformation::scale_and_rotate(bent_cigar_x, bent_cigar_z, bent_cigar_y, Os, Mr, 1.0, 0, 0);

            std::vector<double> hgbat_x(y.begin() + G[1], y.begin() + G[1] + G_nx[1]);
            std::vector<double> hgbat_z(hgbat_x.size());
            std::vector<double> hgbat_y(hgbat_x.size());
            ioh::problem::transformation::scale_and_rotate(hgbat_x, hgbat_z, hgbat_y, Os, Mr, 5.0 / 100.0, 0, 0);

            std::vector<double> rastrigin_x(y.begin() + G[2], y.begin() + G[2] + G_nx[2]);
            std::vector<double> rastrigin_z(rastrigin_x.size());
            std::vector<double> rastrigin_y(rastrigin_x.size());
            ioh::problem::transformation::scale_and_rotate(rastrigin_x, rastrigin_z, rastrigin_y, Os, Mr, 5.12 / 100.0, 0, 0);

            std::vector<double> prepared_y;

            prepared_y.reserve(
                bent_cigar_z.size() \
                + hgbat_z.size() \
                + rastrigin_z.size() \
            );

            prepared_y.insert(prepared_y.end(), bent_cigar_z.begin(), bent_cigar_z.end());
            prepared_y.insert(prepared_y.end(), hgbat_z.begin(), hgbat_z.end());
            prepared_y.insert(prepared_y.end(), rastrigin_z.begin(), rastrigin_z.end());

            return prepared_y;
        }

    public:

        inline static const int meta_problem_id = 1006;
        inline static const std::string meta_name = "CEC_HybridFunction1";

        HybridFunction1(const int instance, const int n_variables) :
            CECProblem(meta_problem_id, instance, n_variables, meta_name)
        {
            this->set_optimum();
        }
    };
}
