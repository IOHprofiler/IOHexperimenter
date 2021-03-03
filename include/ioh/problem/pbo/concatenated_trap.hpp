#pragma once
#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            class Concatenated_Trap : public PBOProblem<Concatenated_Trap>
            {
            protected:
                int k = 5;

                std::vector<double> evaluate(const std::vector<int> &x) override
                {
                    auto result = 0.0;
                    double block_result;
                    auto n_ = x.size();
                    const auto m = n_ / k;
                    for (auto i = 1; i <= m; ++i)
                    {
                        block_result = 0.0;
                        for (auto j = i * k - k; j != i * k; ++j)
                        {
                            block_result += x[j];
                        }
                        if (block_result == k)
                        {
                            result += 1;
                        }
                        else
                        {
                            result += (static_cast<double>(k - 1) - block_result) / static_cast<double>(k);
                        }
                    }
                    const auto remain_k = n_ - m * k;
                    if (remain_k != 0)
                    {
                        block_result = 0.0;
                        // TODO: check this, only with braces this works
                        for (auto j = m * (k - 1); j != n_; ++j)
                        {
                            block_result += x[j];
                        }
                        if (block_result == remain_k)
                        {
                            result += 1;
                        }
                        else
                        {
                            result += static_cast<double>(remain_k - 1 - block_result) / static_cast<double>(remain_k);
                        }
                    }

                    return {result};
                }

            public:
                /**
                 * \brief Construct a new Concatenated_Trap object. Definition refers to
                 *https://doi.org/10.1007/978-3-030-58115-2_49
                 *
                 * \param instance_id The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param dimension The dimensionality of the problem to created, 4 by default.
                 **/
                Concatenated_Trap(const int instance, const int n_variables) :
                    PBOProblem(24, instance, n_variables, "Concatenated_Trap")
                {
                    objective_.x = std::vector<int> (n_variables,1);
                    objective_.y = evaluate(objective_.x);
                }
            };
        } // namespace pbo
    } // namespace problem
} // namespace ioh
