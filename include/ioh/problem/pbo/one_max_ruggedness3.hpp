#pragma once
#include "ioh/problem/utils.hpp"
#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            //! OneMaxRuggedness3 problem id 10
            class OneMaxRuggedness3 final: public PBOProblem<OneMaxRuggedness3>
            {
                std::vector<double> info_;
            protected:
                //! Evaluation method
                double evaluate(const std::vector<int> &x) override
                {
                    auto result = 0.0;
                    for (const auto& xi : x)
                        result += xi;
                    return info_[static_cast<int>(result + 0.5)];
                }

            public:
                /**
                 * \brief Construct a new OneMax_Ruggedness3 object. Definition refers to
                 *https://doi.org/10.1016/j.asoc.2019.106027
                 *
                 * \param instance The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param n_variables The dimensionality of the problem to created, 4 by default.
                 **/
                OneMaxRuggedness3(const int instance, const int n_variables) :
                    PBOProblem(10, instance, n_variables, "OneMaxRuggedness3"),
                    info_(utils::ruggedness3(n_variables))
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
