#pragma once
#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            //! Linear problem id 3
            class Linear final: public PBOProblem<Linear>
            {
                std::vector<double> info_;
            protected:
            
                //! Evaluation method
                double evaluate(const std::vector<int> &x) override
                {
                    auto result = 0.0;
                    for (auto i = 0; i < meta_data_.n_variables; ++i)
                        result += static_cast<double>(x[i]) * (static_cast<double>(i) + 1.0);
                    return result;
                }

            public:
                /**
                 * \brief Construct a new Linear object. Definition refers to https://doi.org/10.1016/j.asoc.2019.106027
                 *
                 * \param instance The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param n_variables The dimensionality of the problem to created, 4 by default.
                 **/
                Linear(const int instance, const int n_variables) : 
                PBOProblem(3, instance, n_variables, "Linear") 
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
