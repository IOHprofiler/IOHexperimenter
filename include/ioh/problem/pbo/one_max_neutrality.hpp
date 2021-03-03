#pragma once
#include "ioh/problem/utils.hpp"
#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            class OneMax_Neutrality : public PBOProblem<OneMax_Neutrality>
            {
            protected:
                std::vector<double> evaluate(const std::vector<int> &x) override
                {
                    auto new_variables = utils::neutrality(x, 3);
                    auto result = 0.0;
                    auto n = new_variables.size();
                    for (auto i = 0; i != n; ++i)
                    {
                        result += new_variables[i];
                    }
                    return {static_cast<double>(result)};
                }

            public:
                /**
                 * \brief Construct a new OneMax_Neutrality object. Definition refers to
                 *https://doi.org/10.1016/j.asoc.2019.106027
                 *
                 * \param instance_id The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param dimension The dimensionality of the problem to created, 4 by default.
                 **/
                OneMax_Neutrality(const int instance, const int n_variables) :
                    PBOProblem(6, instance, n_variables, "OneMax_Neutrality")
                {
                    objective_.x = std::vector<int>(n_variables,1);
                    objective_.y = evaluate(objective_.x);
                }
            };
        } // namespace pbo
    } // namespace problem
} // namespace ioh
