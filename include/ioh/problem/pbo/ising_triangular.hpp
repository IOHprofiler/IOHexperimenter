#pragma once
#include "pbo_problem.hpp"

namespace ioh
{
    namespace problem
    {
        //! PBO namespace
        namespace pbo
        {
            //! IsingTriangular problem id 21
            class IsingTriangular final: public PBOProblem<IsingTriangular>
            {
                static size_t modulo_ising_triangular(const size_t x, const size_t n) { return (x % n + n) % n; }
            protected:
                //! Evaluation method
                double evaluate(const std::vector<int> &x) override
                {
                    auto result = 0.0;
                    size_t neighbors[3];
                    const auto lattice_size = static_cast<size_t>(sqrt(static_cast<double>(meta_data_.n_variables)));

                    for (size_t i = 0; i < lattice_size; ++i)
                    {
                        for (size_t j = 0; j < lattice_size; ++j)
                        {
                            neighbors[0] = x[modulo_ising_triangular(i + 1, lattice_size) * lattice_size + j];
                            neighbors[1] = x[i * lattice_size + modulo_ising_triangular(j + 1, lattice_size)];
                            neighbors[2] = x[modulo_ising_triangular(i + 1, lattice_size) * lattice_size +
                                             modulo_ising_triangular(j + 1, lattice_size)];

                            for (auto neighbor : neighbors)
                            {
                                result += x.at(i * lattice_size + j) * neighbor +
                                    (1 - x.at(i * lattice_size + j)) * (1 - neighbor);
                            }
                        }
                    }
                    return static_cast<double>(result);
                }

            public:
                /**
                 * \brief Construct a new Ising_Triangular object. Definition refers to
                 *https://doi.org/10.1016/j.asoc.2019.106027
                 *
                 * \param instance The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param n_variables The dimensionality of the problem to created, 4 by default.
                 **/
                IsingTriangular(const int instance, const int n_variables) :
                    PBOProblem(21, instance, n_variables, "IsingTriangular")
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
