#pragma once
#include "ioh/problem/utils.hpp"
#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            class OneMax_Epistasis : public PBOProblem<OneMax_Epistasis>
            {
            protected:

                std::vector<double> evaluate(const std::vector<int> &x) override
                {
                    auto new_variables = utils::epistasis(x, 4);
                    auto n = new_variables.size();
                    auto result = 0.0;
                    for (auto i = 0; i != n; ++i)
                    {
                        result += new_variables[i];
                    }
                    return {static_cast<double>(result)};
                }

            public:
                /**
                 * \brief Construct a new OneMax_Epistasis object. Definition refers to
                 *https://doi.org/10.1016/j.asoc.2019.106027
                 *
                 * \param instance_id The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param dimension The dimensionality of the problem to created, 4 by default.
                 **/
                OneMax_Epistasis(const int instance, const int n_variables) :
                    PBOProblem(7, instance, n_variables, "OneMax_Epistasis")
                {
                    objective_.y = evaluate(objective_.x);
                }
            };
        } // namespace pbo
    } // namespace problem
} // namespace ioh
