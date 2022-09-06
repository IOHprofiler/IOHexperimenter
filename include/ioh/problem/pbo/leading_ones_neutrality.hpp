#pragma once
#include "ioh/problem/utils.hpp"
#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            //! LeadingOnesNeutrality problem id 13
            class LeadingOnesNeutrality final: public PBOProblem<LeadingOnesNeutrality>
            {
            protected:
                //! Evaluation method
                double evaluate(const std::vector<int> &x) override
                {
                    auto new_variables = utils::neutrality(x, 3);
                    auto result = 0.0;
                    for (size_t i = 0; i < new_variables.size(); ++i)
                        if (new_variables[i] == 1)
                            result = static_cast<double>(i) + 1.;
                        else
                            break;
                    return result;
                }

            public:
                /**
                 * \brief Construct a new LeadingOnes_Neutrality object. Definition refers to
                 *https://doi.org/10.1016/j.asoc.2019.106027
                 *
                 * \param instance The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param n_variables The dimensionality of the problem to created, 4 by default.
                 **/
                LeadingOnesNeutrality(const int instance, const int n_variables) :
                    PBOProblem(13, instance, n_variables, "LeadingOnesNeutrality")
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
