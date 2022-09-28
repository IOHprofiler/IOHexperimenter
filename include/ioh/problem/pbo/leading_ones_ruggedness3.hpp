#pragma once
#include "ioh/problem/utils.hpp"
#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            //! LeadingOnesRuggedness3 problem id 17
            class LeadingOnesRuggedness3 final: public PBOProblem<LeadingOnesRuggedness3>
            {
                std::vector<double> info_;
            protected:
            
                //! Evaluation method
                double evaluate(const std::vector<int> &x) override
                {
                    auto result = 0;
                    for (auto i = 0; i != meta_data_.n_variables; ++i)
                        if (x[i] == 1)
                            result = i + 1;
                        else
                            break;
                    return info_[static_cast<int>(result + 0.5)];
                }

            public:
                /**
                 * \brief Construct a new LeadingOnes_Ruggedness3 object. Definition refers to
                 *https://doi.org/10.1016/j.asoc.2019.106027
                 *
                 * \param instance The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param n_variables The dimensionality of the problem to created, 4 by default.
                 **/
                LeadingOnesRuggedness3(const int instance, const int n_variables) :
                    PBOProblem(17, instance, n_variables, "LeadingOnesRuggedness3"),
                    info_ (utils::ruggedness3(n_variables))
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
