#pragma once

#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            /**
              * \brief Construct a new OneMax object. Definition refers to https://doi.org/10.1016/j.asoc.2019.106027
              *
              * \param instance_id The instance number of a problem, which controls the transformation
              * performed on the original problem.
              * \param dimension The dimensionality of the problem to created, 4 by default.
              **/

            class OneMax final: public PBOProblem<OneMax>
            {
            protected:
                std::vector<double> evaluate(std::vector<int>& x) override
                {
                    return { std::accumulate(x.begin(), x.end(), 0.0) };
                }

            public:
                OneMax(const int instance, const int n_variables) :
                    PBOProblem(1, instance, n_variables, "OneMax")
                {
                }
            };
        }
    }
}