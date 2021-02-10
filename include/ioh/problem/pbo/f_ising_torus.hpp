#pragma once
#include "pbo_base.hpp"

namespace ioh {
    namespace problem {
        namespace pbo {
            class Ising_Torus : public pbo_base {
            public:
                /**
                       * \brief Construct a new Ising_Torus object. Definition refers to https://doi.org/10.1016/j.asoc.2019.106027
                       * 
                       * \param instance_id The instance number of a problem, which controls the transformation
                       * performed on the original problem.
                       * \param dimension The dimensionality of the problem to created, 4 by default.
                       **/
                Ising_Torus(int instance_id = IOH_DEFAULT_INSTANCE,
                            int dimension = IOH_DEFAULT_DIMENSION)
                    : pbo_base(20, "Ising_Torus", instance_id, dimension) {
                    set_best_variables(1);
                    set_number_of_variables(dimension);
                }

                static int modulo_ising_torus(int x, int N) {
                    return (x % N + N) % N;
                }

                double internal_evaluate(const std::vector<int> &x) override {
                    auto result = 0.0;
                    int neighbors[2];
                    const auto lattice_size = static_cast<int>(sqrt(static_cast<double>(n_)));

                    if (floor(sqrt(static_cast<double>(n_))) != sqrt(static_cast<double>(n_))) {
                        common::log::error(
                            "Number of parameters in the Ising square problem must be a square number");
                    }

                    for (auto i = 0; i < lattice_size; ++i) {
                        for (auto j = 0; j < lattice_size; ++j) {
                            neighbors[0] = x[modulo_ising_torus(i + 1, lattice_size) * lattice_size + j];
                            neighbors[1] = x[lattice_size * i + modulo_ising_torus(j + 1, lattice_size)];
                            for (auto neighbor = 0; neighbor < 2; neighbor++) {
                                result += x[lattice_size * i + j] * neighbors[neighbor] + (
                                    1 - x[i * lattice_size + j]) * (1 - neighbors[neighbor]);
                            }
                        }
                    }
                    return result;
                }

                static Ising_Torus *create(
                    int instance_id = IOH_DEFAULT_INSTANCE,
                    int dimension = IOH_DEFAULT_DIMENSION) {
                    return new Ising_Torus(instance_id, dimension);
                }
            };
        }
    }
}
