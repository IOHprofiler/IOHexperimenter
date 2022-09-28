#pragma once

#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            //! OneMax problem id 1
            class OneMax final: public PBOProblem<OneMax>
            {
            protected:
                //! Evaluation method
                double evaluate(const std::vector<int>& x) override
                {
                    return std::accumulate(x.begin(), x.end(), 0.0);
                }

            public:
                /**
                 * \brief Construct a new OneMax object. Definition refers to https://doi.org/10.1016/j.asoc.2019.106027
                 *
                 * \param instance The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param n_variables The dimensionality of the problem to created, 4 by default.
                 **/
                OneMax(const int instance, const int n_variables) :
                    PBOProblem(1, instance, n_variables, "OneMax")
                {
                    optimum_.x = std::vector<int>(n_variables,1);
                    optimum_.y = evaluate(optimum_.x);
                    optimum_.x = reset_transform_variables(optimum_.x);
                    optimum_.y = transform_objectives(optimum_.y);
                }
            };
        }
    }
}
