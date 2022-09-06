#pragma once
#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            //! IsingRing problem id 19
            class IsingRing final: public PBOProblem<IsingRing>
            {
                static int modulo_ising_ring(const int x, const int n){ return (x % n + n) % n; }
            protected:
            
                //! Evaluation method
                double evaluate(const std::vector<int> &x) override
                {
                    auto result = 0.0;
                    for (auto i = 0; i < meta_data_.n_variables; ++i)
                    {
                        const auto neighbors = x.at(modulo_ising_ring(i - 1, meta_data_.n_variables));
                        result += static_cast<double>(x.at(i)) * neighbors + (1.0 - x.at(i)) * (1.0 - neighbors);
                    }
                    return result;
                }

            public:
                /**
                 * \brief Construct a new Ising_Ring object. Definition refers to
                 *https://doi.org/10.1016/j.asoc.2019.106027
                 *
                 * \param instance The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param n_variables The dimensionality of the problem to created, 4 by default.
                 **/
                IsingRing(const int instance, const int n_variables) :
                    PBOProblem(19, instance, n_variables, "IsingRing")
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
