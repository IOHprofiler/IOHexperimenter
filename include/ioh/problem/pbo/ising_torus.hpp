#pragma once
#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            class Ising_Torus : public PBOProblem<Ising_Torus>
            {
            protected:
                static int modulo_ising_torus(int x, int N) { return (x % N + N) % N; }

                std::vector<double> evaluate(const std::vector<int> &x) override
                {
                    auto result = 0.0;
                    auto n_ = x.size();
                    int neighbors[2];
                    const auto lattice_size = static_cast<int>(sqrt(static_cast<double>(n_)));

                    if (floor(sqrt(static_cast<double>(n_))) != sqrt(static_cast<double>(n_)))
                    {
                        common::log::error("Number of parameters in the Ising square problem must be a square number");
                    }

                    for (auto i = 0; i < lattice_size; ++i)
                    {
                        for (auto j = 0; j < lattice_size; ++j)
                        {
                            neighbors[0] = x[modulo_ising_torus(i + 1, lattice_size) * lattice_size + j];
                            neighbors[1] = x[lattice_size * i + modulo_ising_torus(j + 1, lattice_size)];
                            for (auto neighbor = 0; neighbor < 2; neighbor++)
                            {
                                result += x[lattice_size * i + j] * neighbors[neighbor] +
                                    (1 - x[i * lattice_size + j]) * (1 - neighbors[neighbor]);
                            }
                        }
                    }
                    return { result };
                }

            public:
                /**
                 * \brief Construct a new Ising_Torus object. Definition refers to
                 *https://doi.org/10.1016/j.asoc.2019.106027
                 *
                 * \param instance_id The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param dimension The dimensionality of the problem to created, 4 by default.
                 **/
                Ising_Torus(const int instance, const int n_variables) :
                    PBOProblem(20, instance, n_variables, "Ising_Torus")
                {
                    objective_.x = std::vector<int>(n_variables,1);
                    objective_.y = evaluate(objective_.x);                    
                }
            };
        } // namespace pbo
    } // namespace problem
} // namespace ioh
