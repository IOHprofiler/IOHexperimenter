#pragma once
#include "ioh/problem/utils.hpp"
#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            class LeadingOnes_Epistasis : public PBOProblem<LeadingOnes_Epistasis>
            {
            protected:
                std::vector<double> evaluate(const std::vector<int> &x) override
                {
                    auto new_variables = utils::epistasis(x, 4);
                    auto n = new_variables.size();
                    auto result = 0;
                    for (auto i = 0; i != n; ++i)
                    {
                        if (new_variables[i] == 1)
                        {
                            result = i + 1;
                        }
                        else
                        {
                            break;
                        }
                    }
                    return {static_cast<double>(result)};
                }

            public:
                /**
                 * \brief Construct a new LeadingOnes_Epistasis object. Definition refers to
                 *https://doi.org/10.1016/j.asoc.2019.106027
                 *
                 * \param instance_id The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param dimension The dimensionality of the problem to created, 4 by default.
                 **/
                LeadingOnes_Epistasis(const int instance, const int n_variables) :
                    PBOProblem(14, instance, n_variables, "LeadingOnes_Epistasis")
                {
                    objective_.y = {static_cast<double>(n_variables)};
                }
            };
        } // namespace pbo
    } // namespace problem
} // namespace ioh
