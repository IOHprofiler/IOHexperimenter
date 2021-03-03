#pragma once
#include "ioh/problem/utils.hpp"
#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            class LeadingOnes_Dummy1 : public PBOProblem<LeadingOnes_Dummy1>
            {
            protected:
                std::vector<int> info;

                std::vector<double> evaluate(const std::vector<int> &x) override
                {
                    auto result = 0.0;
                    auto n = this->info.size();
                    for (auto i = 0; i != n; ++i)
                    {
                        if (x[this->info[i]] == 1)
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
                 * \brief Construct a new LeadingOnes_Dummy1 object. Definition refers to
                 *https://doi.org/10.1016/j.asoc.2019.106027
                 *
                 * \param instance_id The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param dimension The dimensionality of the problem to created, 4 by default.
                 **/
                LeadingOnes_Dummy1(const int instance, const int n_variables) :
                    PBOProblem(11, instance, n_variables, "LeadingOnes_Dummy1")
                {
                    info = utils::dummy(n_variables, 0.5, 10000);
                    objective_.x = std::vector<int>(n_variables,1);
                    objective_.y = evaluate(objective_.x);
                }
            };
        } // namespace pbo
    } // namespace problem
} // namespace ioh
