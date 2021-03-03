#pragma once
#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {

            class Ising_Ring : public PBOProblem<Ising_Ring>
            {
            protected:
                static int modulo_ising_ring(int x, int N) { return (x % N + N) % N; }

                std::vector<double> evaluate(const std::vector<int> &x) override
                {
                    auto result = 0.0;
                    auto n_ = x.size();
                    for (auto i = 0; i < n_; ++i)
                    {
                        const auto neig = x[modulo_ising_ring(i - 1, n_)];
                        result += x[i] * neig + (1 - x[i]) * (1 - neig);
                    }
                    return { static_cast<double>(result) };
                }

            public:
                /**
                 * \brief Construct a new Ising_Ring object. Definition refers to
                 *https://doi.org/10.1016/j.asoc.2019.106027
                 *
                 * \param instance_id The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param dimension The dimensionality of the problem to created, 4 by default.
                 **/
                Ising_Ring(const int instance, const int n_variables) :
                    PBOProblem(19, instance, n_variables, "Ising_Ring")
                {
                    objective_.x = std::vector<int>(n_variables,1);
                    objective_.y = evaluate(objective_.x);
                }
            };
        } // namespace pbo
    } // namespace problem
} // namespace ioh
