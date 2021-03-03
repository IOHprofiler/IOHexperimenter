#pragma once
#include "ioh/problem/utils.hpp"
#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            class LeadingOnes_Ruggedness3 : public PBOProblem<LeadingOnes_Ruggedness3>
            {
            protected:
                std::vector<double> info;

                std::vector<double> evaluate(const std::vector<int> &x) override
                {
                    auto result = 0.0;
                    auto n_ = x.size();
                    for (auto i = 0; i != n_; ++i)
                    {
                        if (x[i] == 1)
                        {
                            result = i + 1;
                        }
                        else
                        {
                            break;
                        }
                    }
                    return {this->info[static_cast<int>(result + 0.5)]};
                }

            public:
                /**
                 * \brief Construct a new LeadingOnes_Ruggedness3 object. Definition refers to
                 *https://doi.org/10.1016/j.asoc.2019.106027
                 *
                 * \param instance_id The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param dimension The dimensionality of the problem to created, 4 by default.
                 **/
                LeadingOnes_Ruggedness3(const int instance, const int n_variables) :
                    PBOProblem(17, instance, n_variables, "LeadingOnes_Ruggedness3")
                {
                    info = utils::ruggedness3(n_variables);
                    objective_.x = std::vector<int>(n_variables,1);
                    objective_.y = evaluate(objective_.x);
                }
            };
        } // namespace pbo
    } // namespace problem
} // namespace ioh
