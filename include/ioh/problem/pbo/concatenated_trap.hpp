#pragma once
#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        //! PBO namepsace
        namespace pbo
        {
            //! ConcatenatedTrap problem id 24
            class ConcatenatedTrap final: public PBOProblem<ConcatenatedTrap>
            {
                int k_ = 5;
            protected:
                //! Evaluation method
                double evaluate(const std::vector<int> &x) override
                {
                    auto result = 0.0;
                    double block_result;
                    const auto m = meta_data_.n_variables / k_;
                    for (auto i = 1; i <= m; ++i)
                    {
                        block_result = 0.0;
                        for (auto j = i * k_ - k_; j != i * k_; ++j)
                        {
                            block_result += x[j];
                        }
                        if (block_result == k_)
                        {
                            result += 1;
                        }
                        else
                        {
                            result += ((static_cast<double>(k_) - 1.0) - block_result) / static_cast<double>(k_);
                        }
                    }
                    const auto remain_k = meta_data_.n_variables - m * k_;
                    if (remain_k != 0)
                    {
                        block_result = 0.0;
                        // TODO: check this, only with braces this works
                        for (auto j = m * (k_ - 1); j != meta_data_.n_variables; ++j)
                        {
                            block_result += x[j];
                        }
                        if (block_result == remain_k)
                        {
                            result += 1;
                        }
                        else
                        {
                            result += (static_cast<double>(remain_k) - 1.0 - block_result) / static_cast<double>(remain_k);
                        }
                    }

                    return result;
                }

            public:
                /**
                 * \brief Construct a new Concatenated_Trap object. Definition refers to
                 *https://doi.org/10.1007/978-3-030-58115-2_49
                 *
                 * \param instance The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param n_variables The dimensionality of the problem to created, 4 by default.
                 **/
                ConcatenatedTrap(const int instance, const int n_variables) :
                    PBOProblem(24, instance, n_variables, "ConcatenatedTrap")
                {
                    optimum_.x = std::vector<int> (n_variables,1);
                    optimum_.y = evaluate(optimum_.x);
                    optimum_.x = reset_transform_variables(optimum_.x);
                    optimum_.y = transform_objectives(optimum_.y);
                }
            };
        } // namespace pbo
    } // namespace problem
} // namespace ioh
