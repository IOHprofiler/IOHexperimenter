#pragma once
#include "ioh/problem/utils.hpp"
#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            class LeadingOnes_Ruggedness1 : public PBOProblem<LeadingOnes_Ruggedness1>
            {

            protected:
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
                    return {utils::ruggedness1(result, n_)};
                }

            public:
                /**
                 * \brief Construct a new LeadingOnes_Ruggedness1 object. Definition refers to
                 *https://doi.org/10.1016/j.asoc.2019.106027
                 *
                 * \param instance_id The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param dimension The dimensionality of the problem to created, 4 by default.
                 **/
                LeadingOnes_Ruggedness1(const int instance, const int n_variables) :
                    PBOProblem(15, instance, n_variables, "LeadingOnes_Ruggedness1")
                {
                    objective_.x = std::vector<int>(n_variables,1);
                    objective_.y = {n_variables / 2.0 + 1};
                }
            };
        } // namespace pbo
    } // namespace problem
} // namespace ioh
