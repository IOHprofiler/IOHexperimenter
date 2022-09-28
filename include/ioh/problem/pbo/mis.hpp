#pragma once
#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            //! MIS problem id 22
            class MIS final : public PBOProblem<MIS>
            {
                int number_of_variables_even_;

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
            protected:
                //! Evaluation method
                double evaluate(const std::vector<int> &x) override
                {
                    auto num_of_ones = 0;
                    auto sum_edges_in_the_set = 0;
                    auto number_of_variables_even = meta_data_.n_variables;
                    std::vector<int> ones_array(static_cast<size_t>(number_of_variables_even) + 1);

                    if (number_of_variables_even % 2 != 0)
                        --number_of_variables_even;

                    for (auto index = 0; index < number_of_variables_even; index++)
                        if (x[index] == 1)
                        {
                            ones_array[num_of_ones] = index;
                            num_of_ones += 1;
                        }

                    for (auto i = 0; i < num_of_ones; i++)
                        for (auto j = i + 1; j < num_of_ones; j++)
                            if (is_edge(ones_array[i] + 1, ones_array[j] + 1, number_of_variables_even) == 1)
                                sum_edges_in_the_set += 1;

                    return static_cast<double>(num_of_ones) - static_cast<double>(number_of_variables_even) * sum_edges_in_the_set;
                }

            public:
                /**
                 * \brief Construct a new MIS object. Definition refers to https://doi.org/10.1016/j.asoc.2019.106027
                 *
                 * \param instance The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param n_variables The dimensionality of the problem to created, 4 by default.
                 **/
                MIS(const int instance, const int n_variables) :
                    PBOProblem(22, instance, n_variables, "MIS"),
                    number_of_variables_even_(n_variables % 2 != 0 ? n_variables - 1 : n_variables)
                {
                    optimum_.y = number_of_variables_even_ % 4 == 0
                        ? (number_of_variables_even_ / 2)
                        : (number_of_variables_even_ / 2 + 1);
                    optimum_.y = transform_objectives(optimum_.y);
                    optimum_.y = transform_objectives(optimum_.y);
                }
            };
        } // namespace pbo
    } // namespace problem
} // namespace ioh
