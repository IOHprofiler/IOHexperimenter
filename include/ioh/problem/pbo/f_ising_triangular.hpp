#pragma once
#include "pbo_base.hpp"

namespace ioh {
    namespace problem {
        namespace pbo {
            class Ising_Triangular : public pbo_base {
            public:
                /**
                       * \brief Construct a new Ising_Triangular object. Definition refers to https://doi.org/10.1016/j.asoc.2019.106027
                       * 
                       * \param instance_id The instance number of a problem, which controls the transformation
                       * performed on the original problem.
                       * \param dimension The dimensionality of the problem to created, 4 by default.
                       **/
                Ising_Triangular(int instance_id = IOH_DEFAULT_INSTANCE,
                                 int dimension = IOH_DEFAULT_DIMENSION)
                    : pbo_base(21, "Ising_Triangular", instance_id) {
                    set_best_variables(1);
                    set_number_of_variables(dimension);
                }

                static int modulo_ising_triangular(int x, int N) {
                    return (x % N + N) % N;
                }

                double internal_evaluate(const std::vector<int> &x) override {
                    const auto n = x.size();
                    auto result = 0;
                    int neighbors[3];
                    const auto lattice_size = static_cast<int>(sqrt(
                        static_cast<double>(n)));

                    for (auto i = 0; i < lattice_size; ++i) {
                        for (auto j = 0; j < lattice_size; ++j) {
                            neighbors[0] = x[modulo_ising_triangular(
                                                 i + 1, lattice_size) *
                                             lattice_size + j];
                            neighbors[1] = x[
                                i * lattice_size + modulo_ising_triangular(
                                    j + 1, lattice_size)];
                            neighbors[2] = x[modulo_ising_triangular(
                                                 i + 1, lattice_size) *
                                             lattice_size +
                                             modulo_ising_triangular(
                                                 j + 1, lattice_size)];

                            for (auto neig = 0; neig < 3; neig++) {
                                result += x[i * lattice_size + j] * neighbors[
                                        neig] + (
                                        1 - x[i * lattice_size + j]) *
                                    (1 - neighbors[neig]);
                            }
                        }
                    }
                    return static_cast<double>(result);
                }

                static Ising_Triangular *create(
                    int instance_id = IOH_DEFAULT_INSTANCE,
                    int dimension = IOH_DEFAULT_DIMENSION) {
                    return new Ising_Triangular(instance_id, dimension);
                }
            };
        }
    }
}
