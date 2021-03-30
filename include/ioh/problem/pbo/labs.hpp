#pragma once
#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            class LABS final : public PBOProblem<LABS>
            {
            protected:
                static double correlation(const std::vector<int> x, const int n, int k)
                {
                    int x1, x2;
                    auto result = 0.0;
                    for (auto i = 0; i < n - k; ++i)
                    {
                        if (x[i] == 0)
                            x1 = -1;
                        else
                            x1 = 1;
                        if (x[i + k] == 0)
                            x2 = -1;
                        else
                            x2 = 1;
                        result += x1 * x2;
                    }
                    return result;
                }

                std::vector<double> evaluate(const std::vector<int> &x) override
                {
                    auto result = 0.0;
                    for (auto k = 1; k != meta_data_.n_variables; ++k)
                    {
                        const auto cor = correlation(x, meta_data_.n_variables, k);
                        result += cor * cor;
                    }
                    result = static_cast<double>(meta_data_.n_variables * meta_data_.n_variables) / 2.0 / result;
                    return { static_cast<double>(result) };
                }

            public:
                /**
                 * \brief Construct a new LABS object. Definition refers to https://doi.org/10.1016/j.asoc.2019.106027
                 *
                 * \param instance The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param n_variables The dimensionality of the problem to created, 4 by default.
                 **/
                LABS(const int instance, const int n_variables) : 
                PBOProblem(18, instance, n_variables, "LABS") {

                }
            };
        } // namespace pbo
    } // namespace problem
} // namespace ioh