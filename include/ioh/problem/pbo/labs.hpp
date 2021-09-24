#pragma once
#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            //! LABS
            class LABS final : public PBOProblem<LABS>
            {
                static double correlation(const std::vector<int>& x, const int n, int k)
                {
                    auto result = 0;
                    for (auto i = 0; i < n - k; ++i)
                      result += (x[i] * 2 - 1) * (x[static_cast<size_t>(i) + k] * 2 - 1);
                      
                    return static_cast<double>(result);
                }
            protected:
            
                //! Evaluation method
                double evaluate(const std::vector<int> &x) override
                {
                    auto result = 0.0;
                    for (auto k = 1; k != meta_data_.n_variables; ++k)
                    {
                        const auto cor = correlation(x, meta_data_.n_variables, k);
                        result += cor * cor;
                    }
                    result = (static_cast<double>(meta_data_.n_variables) * meta_data_.n_variables) / 2.0 / result;
                    return result;
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
