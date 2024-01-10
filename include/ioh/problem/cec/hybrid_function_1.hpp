#pragma once

#include "cec_problem.hpp"

//! Namespace encapsulating all elements of the IOHexperimenter project
namespace ioh::problem::cec
{
    //! A class representing the CEC_HybridFunction1 problem, derived from the CECProblem class.
    /*!
     * This class represents a specific optimization problem which is a hybrid of several other functions.
     * It overrides the evaluate and transform_variables methods to provide the specific implementation for this problem.
     */
    class CEC_HybridFunction1 final : public CECProblem<CEC_HybridFunction1>
    {
    protected:

        //! Evaluates the objective function on the transformed input.
        /*!
         * \param prepared_y A vector of transformed variables.
         * \return The value of the objective function evaluated at the point represented by prepared_y.
         */
        double evaluate(const std::vector<double>& prepared_y) override
        {
            double f = hf02(prepared_y);
            return f;
        }

        //! Transforms the input variables according to the problem's specific transformations.
        /*!
         * This method applies several transformations to the input variables, including scaling, rotation, and permutation.
         * It then divides the transformed variables into several groups and applies additional transformations to each group.
         *
         * \param x A vector of input variables.
         * \return A vector of transformed variables, ready to be evaluated by the objective function.
         */
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            auto&& Os = this->variables_shifts_[0];
            auto&& Mr = this->linear_transformations_[0];
            auto&& S = this->input_permutation_;

            const int cf_num = 3;
            int nx = x.size();
            std::vector<double> z(nx);
            std::vector<double> y(nx);
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

            ioh::problem::transformation::variables::scale_and_rotate(x, z, y, Os, Mr, 1.0, 1, 1);

            for (int i = 0; i < nx; ++i) {
                y[i] = z[S[i] - 1];
            }

            std::vector<double> bent_cigar_x(y.begin() + G[0], y.begin() + G[0] + G_nx[0]);
            std::vector<double> bent_cigar_z(bent_cigar_x.size());
            std::vector<double> bent_cigar_y(bent_cigar_x.size());
            ioh::problem::transformation::variables::scale_and_rotate(bent_cigar_x, bent_cigar_z, bent_cigar_y, Os, Mr, 1.0, 0, 0);

            std::vector<double> hgbat_x(y.begin() + G[1], y.begin() + G[1] + G_nx[1]);
            std::vector<double> hgbat_z(hgbat_x.size());
            std::vector<double> hgbat_y(hgbat_x.size());
            ioh::problem::transformation::variables::scale_and_rotate(hgbat_x, hgbat_z, hgbat_y, Os, Mr, 5.0 / 100.0, 0, 0);

            std::vector<double> rastrigin_x(y.begin() + G[2], y.begin() + G[2] + G_nx[2]);
            std::vector<double> rastrigin_z(rastrigin_x.size());
            std::vector<double> rastrigin_y(rastrigin_x.size());
            ioh::problem::transformation::variables::scale_and_rotate(rastrigin_x, rastrigin_z, rastrigin_y, Os, Mr, 5.12 / 100.0, 0, 0);

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

        //! A constant representing the ID of this problem in the meta-problem series.
        inline static const int meta_problem_id = 1006;

        //! A constant representing the name of this problem.
        inline static const std::string meta_name = "CEC_HybridFunction1";

        //! Constructor for the CEC_HybridFunction1 class.
        /*!
         * Initializes a new instance of the CEC_HybridFunction1 class, setting up the problem with the specified instance and number of variables.
         *
         * \param instance The instance number of the problem.
         * \param n_variables The number of variables for the problem.
         */
        CEC_HybridFunction1(const int instance, const int n_variables) :
            CECProblem(meta_problem_id, instance, n_variables, meta_name)
        {
            this->set_optimum();
        }
    };
}
