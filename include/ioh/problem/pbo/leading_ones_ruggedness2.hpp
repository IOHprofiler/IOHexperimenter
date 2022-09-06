#pragma once
#include "ioh/problem/utils.hpp"
#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {   
            //! LeadingOnesRuggedness2 problem id 16
            class LeadingOnesRuggedness2 final: public PBOProblem<LeadingOnesRuggedness2>
            {
                double evaluate(const std::vector<int> &x) override
                {
                    auto result = 0;
                    for (auto i = 0; i != meta_data_.n_variables; ++i)
                        if (x[i] == 1)
                            result = i + 1;
                        else
                            break;
                    return utils::ruggedness2(result, meta_data_.n_variables);
                }

            public:
                /**
                 * \brief Construct a new LeadingOnes_Ruggedness2 object. Definition refers to
                 *https://doi.org/10.1016/j.asoc.2019.106027
                 *
                 * \param instance The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param n_variables The dimensionality of the problem to created, 4 by default.
                 **/
                LeadingOnesRuggedness2(const int instance, const int n_variables) :
                    PBOProblem(16, instance, n_variables, "LeadingOnesRuggedness2")
                {
                    optimum_.x = std::vector<int>(n_variables,1);
                    optimum_.y = evaluate(optimum_.x);
                    optimum_.x = reset_transform_variables(optimum_.x);
                    optimum_.y = transform_objectives(optimum_.y);
                }
            };
        } // namespace pbo
    } // namespace problem
} // namespace ioh
