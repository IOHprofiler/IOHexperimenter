#pragma once
#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            class MIS final: public PBOProblem<MIS>
            {
            protected:
                static int is_edge(const int i, const int j, const int problem_size)
                {
                    if (i != problem_size / 2 && j == i + 1)
                        return 1;
                    if (i <= static_cast<int>(problem_size) / 2 - 1 && j == i + static_cast<int>(problem_size) / 2 + 1)
                        return 1;
                    if (i <= static_cast<int>(problem_size) / 2 && i >= 2 &&
                        j == i + static_cast<int>(problem_size) / 2 - 1)
                        return 1;
                    return 0;
                }


                std::vector<double> evaluate(const std::vector<int> &x) override
                {
                    auto result = 0.0;
                    auto num_of_ones = 0;
                    auto sum_edges_in_the_set = 0;
                    auto number_of_variables_even = meta_data_.n_variables;
                    std::vector<int> ones_array(number_of_variables_even + 1);

                    if (meta_data_.n_variables % 2 != 0)
                    {
                        number_of_variables_even = meta_data_.n_variables - 1;
                    }

                    for (auto index = 0; index < number_of_variables_even; index++)
                    {
                        if (x[index] == 1)
                        {
                            ones_array[num_of_ones] = index;
                            num_of_ones += 1;
                        }
                    }

                    for (auto i = 0; i < num_of_ones; i++)
                    {
                        for (auto j = i + 1; j < num_of_ones; j++)
                        {
                            if (is_edge(ones_array[i] + 1, ones_array[j] + 1, number_of_variables_even) == 1)
                            {
                                sum_edges_in_the_set += 1;
                            }
                        }
                    }
                    result = num_of_ones - number_of_variables_even * sum_edges_in_the_set;
                    return {static_cast<double>(result)};
                }

            public:
                /**
                 * \brief Construct a new MIS object. Definition refers to https://doi.org/10.1016/j.asoc.2019.106027
                 *
                 * \param instance The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param n_variables The dimensionality of the problem to created, 4 by default.
                 **/
                MIS(const int instance, const int n_variables) : PBOProblem(22, instance, n_variables, "MIS")
                {
                    int n_ = n_variables;
                    double y;
                    if (n_ % 2 != 0)
                    {
                        --n_;
                    }
                    y = n_ % 4 == 0 ? (n_ / 2) : (n_ / 2 + 1);
                    objective_.y = {y};
                }
            };
        } // namespace pbo
    } // namespace problem
} // namespace ioh
