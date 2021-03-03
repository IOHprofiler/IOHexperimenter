#pragma once
#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            class Ising_Triangular : public PBOProblem<Ising_Triangular>
            {
            protected:
                static int modulo_ising_triangular(int x, int N) { return (x % N + N) % N; }

                std::vector<double> evaluate(const std::vector<int> &x) override
                {
                    auto result = 0.0;
                    auto n_ = x.size();
                    int neighbors[3];
                    const auto lattice_size = static_cast<int>(sqrt(static_cast<double>(n_)));

                    for (auto i = 0; i < lattice_size; ++i)
                    {
                        for (auto j = 0; j < lattice_size; ++j)
                        {
                            neighbors[0] = x[modulo_ising_triangular(i + 1, lattice_size) * lattice_size + j];
                            neighbors[1] = x[i * lattice_size + modulo_ising_triangular(j + 1, lattice_size)];
                            neighbors[2] = x[modulo_ising_triangular(i + 1, lattice_size) * lattice_size +
                                             modulo_ising_triangular(j + 1, lattice_size)];

                            for (auto neig = 0; neig < 3; neig++)
                            {
                                result += x[i * lattice_size + j] * neighbors[neig] +
                                    (1 - x[i * lattice_size + j]) * (1 - neighbors[neig]);
                            }
                        }
                    }
                    return { static_cast<double>(result) };
                }

            public:
                /**
                 * \brief Construct a new Ising_Triangular object. Definition refers to
                 *https://doi.org/10.1016/j.asoc.2019.106027
                 *
                 * \param instance_id The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param dimension The dimensionality of the problem to created, 4 by default.
                 **/
                Ising_Triangular(const int instance, const int n_variables) :
                    PBOProblem(21, instance, n_variables, "Ising_Triangular")
                {
                    objective_.x = std::vector<int>(n_variables,1);
                    objective_.y = evaluate(objective_.x);
                }
            };
        } // namespace pbo
    } // namespace problem
} // namespace ioh
